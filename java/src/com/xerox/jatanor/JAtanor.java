/*
 *  NAVER LABS Europe
 *  project ATANOR
 *  Copyright (C) 2017 NAVER LABS Europe, All Rights Reserved
 *
 */

package com.xerox.jatanor;

import java.io.File;
import java.util.Vector;

/**
 * Description of the Class
 *
 * @author roux @created April 10, 2017
 */
public class JAtanor {

   /**
     * Constructor for the JAtanor object
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

    private native String[] ExecuteFunctionArrayImplementation(int handler, String code, String[] args)
            throws Exception;

    /**
     * Execute an Atanor program from String
     *
     * @param code of the KiF program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return 
     * @exception Exception
     */
    public synchronized String[] ExecuteFunctionArray(int handler, String code,String[] args) throws Exception {
        return ExecuteFunctionArrayImplementation(handler, code, args);
    }

    //------------------------------------------------------------------------

    private native int CleanImplementation(int handler) throws Exception;

    /**
     * Clean One Athanor Program
     *
     */
    public synchronized int Clean(int handler) throws Exception {
        return CleanImplementation(handler);
    }

    //------------------------------------------------------------------------

    private native int CleanAllImplementation() throws Exception;

    /**
     * Clean all Athanor Programs
     *
     */
    public synchronized int CleanAll() throws Exception {
        return CleanAllImplementation();
    }
}
