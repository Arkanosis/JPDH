package com.arkanosis.jpdh;

public class Query implements AutoCloseable {
    public native Counter addCounter(String fullCounterPath) throws JPDHException;
    public native void collectData() throws JPDHException;
    public native void close() throws JPDHException;

    private transient long __native_ref;
}
