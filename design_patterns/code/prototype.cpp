#include <iostream>

class Keyframe {
   public:
    virtual Keyframe* clone() = 0;
    virtual void print() = 0;
};

class SLAMKeyframe : public Keyframe {
    int id_;
    float x_, y_;

   public:
    SLAMKeyframe(int id, float x, float y) : id_(id), x_(x), y_(y) {}

    SLAMKeyframe* clone() override { return new SLAMKeyframe(*this); }

    void print() override {
        std::cout << "Keyframe id=" << id_ << " pos=(" << x_ << "," << y_
                  << ")\n";
    }
};

int main() {
    SLAMKeyframe kf(1, 3.5, 2.1);
    SLAMKeyframe* kf_clone = kf.clone();

    kf.print();
    kf_clone->print();  // same data, different object

    delete kf_clone;
}