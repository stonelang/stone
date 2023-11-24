#include <stdio.h>


class StatisticTracer {

}
class CompilerStatisticTracer : StatisticTracer {
public:
	~CompilerStatisticTracer() {
		engine.SaveTracer(*this);
	}
}
class StatisticEngine {
	SmallVector<Statistic> stats;


public:
	void SaveTracer(StatisticTracer* tracer) {
		stats.push_back(new Statistic(....))
	}

}