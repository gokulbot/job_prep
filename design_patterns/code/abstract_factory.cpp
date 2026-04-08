#include <iostream>

class Optimizer {
   public:
    virtual void optimize() = 0;
};
class FeatureExtractor {
   public:
    virtual void extract() = 0;
};

class CeresOptimizer : public Optimizer {
    void optimize() override { std::cout << "Ceres optimizing\n"; }
};
class GFTTExtractor : public FeatureExtractor {
    void extract() override { std::cout << "GFTT extracting\n"; }
};

class SuperPointExtractor : public FeatureExtractor {
    void extract() override { std::cout << "Superpoint  extracting\n"; }
};
class SLAMFactory {
   public:
    virtual Optimizer* createOptimizer() = 0;
    virtual FeatureExtractor* createExtractor() = 0;
};

class ClassicalSLAMFactory : public SLAMFactory {
    Optimizer* createOptimizer() override { return new CeresOptimizer(); }
    FeatureExtractor* createExtractor() override { return new GFTTExtractor(); }
};

class DeepSLAMFactory : public SLAMFactory {
    Optimizer* createOptimizer() override { return new CeresOptimizer(); }
    FeatureExtractor* createExtractor() override {
        return new SuperPointExtractor();
    }
};

int main() {
    SLAMFactory* factory = new ClassicalSLAMFactory();
    factory->createOptimizer()->optimize();
    factory->createExtractor()->extract();

    delete factory;
    factory = new DeepSLAMFactory();
    factory->createOptimizer()->optimize();
    factory->createExtractor()->extract();

    delete factory;
}