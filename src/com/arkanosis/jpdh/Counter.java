package com.arkanosis.jpdh;

public class Counter {
    public native double getDoubleValue() throws JPDHException;
    public native int getIntegerValue() throws JPDHException;
    public native long getLongValue() throws JPDHException;
    public native void remove() throws JPDHException;

    private transient long __native_ref;
}
