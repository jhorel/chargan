package com.cypress.academy.ble101_robot;

/**
 * Created by jhorel on 2016-04-07.
 */
public class AccUtils {


    public static double Tilt(Vector v){
        double z = v.getZ();
        double modulus = Modulus(v);
        double angle = Math.acos(z/modulus);
        return angle*(57.295779513);
    }

    public static double Modulus(Vector v) {
        double x = v.getX();
        double y = v.getY();
        double z = v.getZ();

        return Math.sqrt(x*x + y*y + z*z);
    }

    public static double Angle (Vector v1, Vector v2){
        double m1 = Modulus(v1);
        double m2 = Modulus(v2);
        double dot  = v1.getX()*v2.getX() + v1.getY()*v2.getY() +v1.getZ()*v2.getZ();
        double angle = Math.acos(dot/(m1 * m2 ));
        return angle*(57.295779513);// in degrees
    }
}
