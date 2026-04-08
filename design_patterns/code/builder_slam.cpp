#include <iostream>

class SLAMPipeline {
   public:
    bool with_loop_closure = false;

    bool use_imu = false;

    bool with_stereo = false;

    void print() {
        std::cout << "Loop Closure :" << with_loop_closure << "\n"
                  << "Use_imu :" << use_imu << "\n"
                  << "With_stereo :" << with_stereo << "\n";
    }
};

class SLAMBuilder {
   private:
    SLAMPipeline slam_;

   public:
    SLAMBuilder& withLoopClosure(bool val) {
        slam_.with_loop_closure = val;
        return *this;
    }

    SLAMBuilder& withIMU(bool val) {
        slam_.use_imu = val;
        return *this;
    }

    SLAMBuilder& withStereo(bool val) {
        slam_.with_stereo = val;
        return *this;
    }

    SLAMPipeline& build() { return slam_; }
};

int main() {
    SLAMPipeline pipeline = SLAMBuilder()
                                .withLoopClosure(true)
                                .withIMU(true)
                                .withStereo(true)
                                .build();
    pipeline.print();
}