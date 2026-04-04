#include <iostream>
#include <memory>

class SLAMBackendInterface {
   public:
    virtual void getPose() = 0;
    virtual ~SLAMBackendInterface() = default   ;
};

class VINSFusion : public SLAMBackendInterface {
   public:
    void getPose() override;
};

class ORBSALM3 : public SLAMBackendInterface {
   public:
    void getPose() override;
};

class SLAMBackendContext {
   public:
    void getPose();
    void setBackend(std::unique_ptr<SLAMBackendInterface> backend);

   private:
    std::unique_ptr<SLAMBackendInterface> slam_backend_;
};

void VINSFusion::getPose() { std::cout << "got ViNS Pose " << "\n"; }
void ORBSALM3::getPose() { std::cout << "got ORBSALM3 Pose " << "\n"; }

void SLAMBackendContext::setBackend(
    std::unique_ptr<SLAMBackendInterface> backend) {
    slam_backend_ = std::move(backend);
}

void SLAMBackendContext::getPose() {
    if (slam_backend_) {
        slam_backend_->getPose();
    }
}