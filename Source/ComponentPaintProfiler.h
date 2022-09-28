#ifndef JUCE_MAJOR_VERSION
#include <JuceHeader.h>
#endif

/*
* you may add your own meta data to tests or customize results by overriding processResults 
* and using protected getters for info about the last test
*/
class JuceComponentRepaintProfiler
{
public:
	JuceComponentRepaintProfiler(juce::File newResultLocation = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory).getChildFile("JuceComponentRepaintData"));
	/*
	* avoided using templates so the compiler wont optimise stuff that wont be optimized in real projects
	* note:
	* - Im afraid this isnt extremely accurate since the component image will probably stay in higher cache levels than in a real project
	*/
	void measureRepaints(juce::Component* componentToTest);

	//change these for testing
	int maxWidth = 1000;
	int widthTestIntervalInPx= 250;
	int heightTestIntervalInPx= 250;
	int maxHeight = 1000;
	int numRuns = 100;
	std::vector<int> repaintIntervalsToTest{ 0, 1, 32, 64 }; 

protected:
	bool getLastBuffered()		{ return isBuffered; }
	int  getLastPaintInterval() { return repaintInterval; }

	void storeResultsAsCSV(juce::StringPairArray& testData);

private:
	virtual void processResults(std::vector<double> results);
	
	void prepareCsvFile(juce::StringPairArray& testData);
	juce::Image setCompSize(const int width, const int height);
	double getMsSinceTick(const juce::uint64& startTicks);
	std::vector<double> getResults(juce::Graphics& g);
	
	juce::File resultFolder;
	juce::Component* cComp = nullptr;

	juce::File csvFile;

	//information about last completed test
	//these are set internally and used to set metadata in processResults
	int width = -1;
	int height = -1;
	bool isBuffered = false;
	int repaintInterval = -1;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceComponentRepaintProfiler)
};