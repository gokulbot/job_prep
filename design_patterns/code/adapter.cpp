#include <iostream>

class SensorInterface {
   public:
    virtual std::string getData() = 0;
};

class LegacyGPS {
   public:
    std::string readRAW() { return "$GPGGA, 123456... "; }
};

class GPSAdapter : public SensorInterface {
   private:
    LegacyGPS legacy_gps_;

   public:
    std::string getData() override { return legacy_gps_.readRAW(); }
};

int main() {
    SensorInterface* sensor = new GPSAdapter();
    std::cout << sensor->getData() << "\n";
    delete sensor;
}
