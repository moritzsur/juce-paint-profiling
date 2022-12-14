A quick way to measure a components paint() performance and store the result as .csv for later analysis.
Because of memory alignment and branch prediction stuff, I wouldnt use it to compare bufferedToImage true/false for components.

By default it stores the csv files in userDocsDir/JuceComponentRepaintData

How to use:
```cpp
	//default settings

    juce::Component testComp;
    testComp.setName("TestComp");
    JuceComponentRepaintProfiler tester;
    tester.measureRepaints(&testComp);

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
    tester.measureRepaints(&testComp);
```

warning: to test the same component twice with different settings, rename it.
Otherwise the new results will be added to the existing csv file.
```cpp
    //testing the same comp with different settings

    JuceComponentRepaintProfiler tester;

    TestComp testComp;

    testComp.setName("TestComp");
    tester.measureRepaints(&testComp);

    testComp.setOpaque(true);
    testComp.setName("TestComp Opaque")
    tester.measureRepaints(&testComp);
```

TODO:
- measure component tree structures paint performance