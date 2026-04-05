#include <iostream>
#include <memory>

enum class SLAMTypes{eORBSLAM3, eViNSFusion};

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
    void setBackend(SLAMTypes type);

   private:
    std::unique_ptr<SLAMBackendInterface> slam_backend_;
};

void VINSFusion::getPose() { std::cout << "got ViNS Pose " << "\n"; }
void ORBSALM3::getPose() { std::cout << "got ORBSALM3 Pose " << "\n"; }

void SLAMBackendContext::setBackend(SLAMTypes type) {

    switch(type){

        case SLAMTypes::eORBSLAM3:
            slam_backend_ = std::make_unique<ORBSALM3>();
            break;

        case SLAMTypes::eViNSFusion:
            slam_backend_ = std::make_unique<VINSFusion>();
            break;
        default:
            slam_backend_ = std::make_unique<VINSFusion>();
            break;
    }   

}

void SLAMBackendContext::getPose() {
    if (slam_backend_) {
        slam_backend_->getPose();
    }
}

  int main() {                                                                                                  
      SLAMBackendContext ctx;                                                                                   
      ctx.setBackend(SLAMTypes::eORBSLAM3);                                                                   
      ctx.getPose();                                                                                            
                                                            
      ctx.setBackend(SLAMTypes::eViNSFusion);                                                                   
      ctx.getPose();                                                                                            
  }                               
    