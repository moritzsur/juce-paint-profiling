#include "ComponentPaintProfiler.h"

JuceComponentRepaintProfiler::JuceComponentRepaintProfiler(juce::File newResultFolder) :
	resultFolder(newResultFolder)
{
	jassert(resultFolder.existsAsFile() == false);

	if (!resultFolder.isDirectory())
		resultFolder.createDirectory();
}

void JuceComponentRepaintProfiler::testComponent(juce::Component* componentToTest)
{
	jassert(componentToTest != nullptr);
	jassert(componentToTest->getName() != ""); //you need to set your components name so a file can be created
	cComp = componentToTest;

	juce::Rectangle<int> rect;

	for (auto nextRepaintInteval : repaintIntervalsToTest)
	{
		repaintInterval = nextRepaintInteval;
		width = maxWidth;

		while (width > 0)
		{
			height = maxHeight;

			while (height > 0)
			{
				auto img = setCompSize(width, height);
				juce::Graphics g(img);

				isBuffered = true;
				const auto bufferedResults = getResults(g);
				processResults(bufferedResults);

				isBuffered = false;
				const auto nonBufferedResults = getResults(g);
				processResults(nonBufferedResults);

				height -= heightTestIntervalInPx;
			}

			width -= widthTestIntervalInPx;
		}
	}
}

juce::Image JuceComponentRepaintProfiler::setCompSize(const int width, const int height)
{
	cComp->setSize(width, height);
	return juce::Image(juce::Image::PixelFormat::ARGB, maxWidth, maxHeight, true);
}

double JuceComponentRepaintProfiler::getMsSinceTick(const juce::uint64& startTicks)
{
	const auto scaler = 1.0 / static_cast<double> (juce::Time::getHighResolutionTicksPerSecond());
	const auto msTaken = static_cast<double> (juce::Time::getHighResolutionTicks() - startTicks) * scaler * 1000.0;
	return msTaken;
}

std::vector<double> JuceComponentRepaintProfiler::getResults(juce::Graphics& g)
{
	auto results = std::vector<double>();
	
	//if interval is smaller than run, the image will only be repainted once at the beginning
	const auto interval = repaintInterval < 1 ? numRuns : repaintInterval;

	cComp->setBufferedToImage(isBuffered);

	for (int i = 0; i < numRuns; i++)
	{
		const auto startTicks = juce::Time::getHighResolutionTicks();

		if (i % interval == 0)
			cComp->repaint();

		cComp->paint(g);

		const auto msTaken = getMsSinceTick(startTicks);
		results.push_back(msTaken);
	}

	return results;
}

void JuceComponentRepaintProfiler::processResults(std::vector<double> results)
{
	using Str = juce::String;

	//calculate relevant data
	auto avg = 0.0;
	auto min = DBL_MAX;
	auto max = DBL_MIN;

	for (auto result : results)
	{
		avg += result;
		min = juce::jmin(min, result);
		max = juce::jmax(max, result);
	}
	avg /= results.size();

	//set tags (will be colums in csv file)
	juce::StringPairArray testData;
	testData.set("NumRuns", Str(numRuns));
	testData.set("PaintInterval", Str(getLastPaintInterval()));
	testData.set("Width", Str(getLastWidth()));
	testData.set("Height", Str(getLastHeight()));
	testData.set("IsBuffered", Str(getLastBuffered() ? "true" : "false"));
	testData.set("MinMS", Str(min));
	testData.set("MaxMS", Str(max));
	testData.set("AvgMS", Str(avg));
	
	//store data for later use
	storeResultsAsCSV(testData);
}

void JuceComponentRepaintProfiler::storeResultsAsCSV(juce::StringPairArray& testData)
{
	using Str = juce::String;

	auto fileName = juce::File::createLegalFileName(cComp->getName() + "Results");
	if (csvFile == juce::File() ||csvFile.getFileNameWithoutExtension() != fileName)
	{
		csvFile = resultFolder.getNonexistentChildFile(fileName, ".csv");
		jassert(!csvFile.exists());
		csvFile.create();
		
		auto topLine = Str();
		for (auto& key : testData.getAllKeys())
		{
			jassert(!key.containsAnyOf(",;"));
			topLine = topLine + key + ",";
		}
		topLine = topLine.dropLastCharacters(1) + "\n";

		csvFile.appendText(topLine, true, true);
	}

	auto csvStr = csvFile.loadFileAsString();
	const auto firstLine = csvStr.upToFirstOccurrenceOf("\r\n", false, true);
	const auto keys = juce::StringArray::fromTokens(firstLine, ",;", "");
	jassert(keys.size() == testData.size());

	//create csv formatted string and save it
	auto strToAppend = Str();
	for (int i = 0; i < keys.size(); i++)
	{
		auto key = keys[i];
		auto value = testData.getValue(key, "");

		if (value == "")
		{
			jassertfalse;
			continue;
		}

		strToAppend += value + Str(",");
	}
	strToAppend = strToAppend.dropLastCharacters(1) + "\n";
	csvFile.appendText(strToAppend, true);
}
