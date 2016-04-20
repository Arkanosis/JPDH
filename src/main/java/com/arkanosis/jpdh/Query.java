package com.arkanosis.jpdh;

public class Query implements AutoCloseable {

    public Counter addCounter(String fullPath) throws JPDHException {
        Counter counter = nAddCounter(fullPath);
        counter.fullPath = fullPath;
        return counter;
    }

    public native void removeCounter(Counter counter) throws JPDHException;

    public native void collectData() throws JPDHException;

    public native void close() throws JPDHException;

    public String getDataSource() {
        return dataSource;
    }

    private native Counter nAddCounter(String fullPath) throws JPDHException;

    protected String dataSource;
    private transient long __native_ref;
}
