#ifndef SUBPIXEL_TEST_H
#define SUBPIXEL_TEST_H

#include <QObject>
#include <QtTest/QtTest>
#include <pointing/transferfunctions/SubPixelFunction.h>

using namespace pointing;
using namespace std;

/*
 * Unit tests for SubPixelFunction which is described in this paper:
 * http://interaction.lille.inria.fr/~roussel/publications/2012-UIST-subpixel.pdf
 */
class SubPixelTest : public QObject
{
  Q_OBJECT
  PointingDevice *input;
  DisplayDevice *output;
  SubPixelFunction *func;

private slots:

  // This configuration of mouse, display and subpixeling is taken from the abovementioned paper.
  // Since there are a lot of variables taken into account in the computations
  // and lots of conditions we need to make sure that the behavior of SubPixelFunction is as expected.
  void initTestCase()
  {
    input = PointingDevice::create("dummy:?cpi=4000&hz=500");
    output = DisplayDevice::create("dummy:?bw=1680&bh=1050&ppi=90&hz=60");
    URI funcURI("naive:?");
    func = new SubPixelFunction("subpixel:?cardinality=1000&widgetSize=90&debugLevel=1&resHuman=1000",
                                    funcURI, input, output);
  }

  void NaiveGain()
  {
    PointingDevice *i = PointingDevice::create("dummy:?cpi=400&hz=125");
    DisplayDevice *o = DisplayDevice::create("dummy:?ppi=90");
    SubPixelFunction *f = new SubPixelFunction("subpixel:?cardinality=100", "naive:?", i, o);
    double dxP, dyP;
    f->applyd(5, 5, &dxP, &dyP);
    QCOMPARE(dxP, 5.);
    QCOMPARE(dyP, 5.);
    delete i; delete o; delete f;
  }

  void HumanResolution()
  {
    func->setHumanResolution(0);
    QVERIFY(func->getHumanResolution() != 0);
    func->setHumanResolution(1000);
    QCOMPARE(func->getHumanResolution(), 1000);
  }

  void PixelContains44SubPixels()
  {
    func->clearState();
    double dxP, dyP;
    TimeStamp::inttime now = TimeStamp::createAsInt();
    int i = 0;
    for (double sum = 0.; sum < 1 && i < 50; i++, sum += dxP)
    {
      func->applyd(1, 0, &dxP, &dyP, now);
      // Add this delta because we want to apply the minimum (optimal) gain
      // to the input and it is applied when the input velocity is small.
      now += TimeStamp::one_second;
    }
    QCOMPARE(i, 45);
  }
  /*
  void Cardinality1000widgetSize50()
  {
    double dxP, dyP;
    func->setCardinalitySize(1000, 50);
    func->clearState();
    func->applyd(5, 5, &dxP, &dyP);
    QCOMPARE(dxP, 5.);
    QCOMPARE(dyP, 5.);
  }
  */
  void OutputEqualsInput()
  {
    double dxP, dyP;
    func->setCardinalitySize(1000, 5000000);
    func->clearState();
    func->applyd(5, 5, &dxP, &dyP);
    QVERIFY2(dxP == 5. && dyP == 5., "Output cannot be more than the input with naive gain = 1");
  }

  // At the end
  void cleanupTestCase()
  {
    delete func;
    delete input;
    delete output;
  }
};

#endif // SUBPIXEL_TEST_H

QTEST_MAIN(SubPixelTest)
