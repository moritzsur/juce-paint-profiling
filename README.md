A quick way to measure a components paint() performance and store the result as .csv for later analysis.
Because of memory alignment and branch prediction stuff, I wouldnt use it to compare bufferedToImage true/false for components.

By default it stores the csv files in userDocsDir/JuceComponentRepaintData

How to use:
```cpp
	//default settings
    juce::Component testComp;
    testComp.setName("TestComp");
    JuceComponentRepaintProfiler tester;
    tester.testComponent(&testComp);

```

```cpp
    //custom settings
    JuceComponentRepaintProfiler tester;

    //repaints every 4th and 8th repaint call
    tester.repaintIntervalsToTest = {4, 8};

    //testing for the sizes 1000x1000, 1000x500, 500x1000, 500x500
    tester.maxWidth = 1000;
    tester.widthTestIntervalInPx = 500;
    tester.maxHeight = 1000;
    tester.heightTestIntervalInPx = 500;

    //testing each setting with 50 repetitions
    tester.numRuns = 50;

    juce::Component testComp;
    testComp.setName("TestComp");
    tester.testComponent(&testComp);
```