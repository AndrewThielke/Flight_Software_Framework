#ifndef GNC_H
#define GNC_H

class GNC {
public:
    void initialize();
    void update();
    void adjustThrust(double deltaV);
};

#endif
