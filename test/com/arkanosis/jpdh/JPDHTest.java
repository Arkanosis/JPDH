package com.arkanosis.jpdh;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

public class JPDHTest {
    public static void main(String[] args) throws Exception {
        try (Query query = JPDH.openQuery()) {
            Counter diskCounter = query.addCounter("\\PhysicalDisk(_Total)\\Disk Read Bytes/sec");
            Counter cpuCounter = query.addCounter("\\PID(8584)\\% User Time");
            query.collectData();
            for (int nbLoops = 3; nbLoops > 0; --nbLoops) {
                try {
                    Thread.sleep(10000);
                } catch (InterruptedException e) {
                    // Don't care...
                }
                query.collectData();
                System.out.println(diskCounter.getDoubleValue());
                System.out.println(cpuCounter.getIntegerValue());
            }
            query.removeCounter(diskCounter);
        }
    }
}
