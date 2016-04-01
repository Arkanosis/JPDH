package com.arkanosis.jpdh;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

public class JPDH {
    static {
        try {
            loadLibrary("jnijpdh");
        } catch (IOException e) {
            throw new ExceptionInInitializerError(e);
        }
    }

    public static native Query openQuery(String dataSource) throws JPDHException;

    public static Query openQuery() throws JPDHException {
        return openQuery(null);
    }

    private static void loadLibrary(String name) throws IOException {
        File library = File.createTempFile(name, ".dll");
        Files.copy(
            JPDH.class.getResourceAsStream("/" + System.mapLibraryName(name)),
            library.toPath(),
            StandardCopyOption.REPLACE_EXISTING
        );
        System.load(library.getAbsolutePath());
    }
}
