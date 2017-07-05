/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project ATANOR
 *  Copyright (C) 2017 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */

package com.xerox.jatanor;

import java.io.File;
import java.util.Vector;

/**
 * Description of the Class
 *
 * @author roux @created January 23, 2017
 */
public class JAtanor {

   /**
     * Constructor for the JAtano object
     */
    public JAtanor() {
		DynLoaderFromJar.loadLibFromJar();
    }
    //------------------------------------------------------------------------

    private native int LoadProgramImplementation(String filename,String args) throws Exception;

    /**
     * Load an Atanor program
     *
     * @param filename of the Atanor program to load
     * @param args arguments as a string, where each parameter is separated with a space
     * @return 
     * @exception Exception
     */
    public synchronized int LoadProgram(String filename,String args) throws Exception {
        return LoadProgramImplementation(filename, args);
    }
    //------------------------------------------------------------------------

    private native String ExecuteFunctionImplementation(int handler, String code, String[] args)
            throws Exception;

    /**
     * Execute an Atanor program from String
     *
     * @param code of the KiF program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return 
     * @exception Exception
     */
    public synchronized String ExecuteFunction(int handler, String code,String[] args) throws Exception {
        return ExecuteFunctionImplementation(handler, code, args);
    }
    //------------------------------------------------------------------------

}
