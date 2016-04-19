package com.arkanosis.jpdh;

public class Counter {
    public native double getDoubleValue() throws JPDHException;

    public native int getIntegerValue() throws JPDHException;

    public native long getLongValue() throws JPDHException;

    public String getFullPath() {
        return fullPath;
    }

    protected String fullPath;
    private transient long __native_ref;
}
