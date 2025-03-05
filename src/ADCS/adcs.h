#ifndef ADCS_H
#define ADCS_H

class ADCS {
public:
    void initialize();
    void update();
    void adjustOrientation(double roll, double pitch, double yaw);
};

#endif
