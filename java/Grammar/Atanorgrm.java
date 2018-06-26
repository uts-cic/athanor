import com.xerox.jatanor.*;
import java.io.*;
import java.util.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;

/**
*  Description of the Class
*
* @author     roux
* @created    30 May 2017
*/
public class Atanorgrm {
	String grm;
    int handler;
	JAtanor jatan;
	
	public Atanorgrm() {
        try {
            // create a JAtanor object to load the dynamic library in the VM
			jatan=new JAtanor();
			handler=-1;	
        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
	}
	
	public void Load(String grm) {
		try {
			System.out.println("GRM="+grm+":"+handler);
			handler=jatan.LoadProgram(grm,"");
		}
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
		}			
	}
	
	public String[] Execute(String json) {
		String[] result;
        try {
            // create a JAtanor object to load the dynamic library in the VM
            String[] arg=new String[1];
            arg[0]=json;
            result=jatan.ExecuteFunctionArray(handler, "Apply",arg);
        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
            result=new String[1];
        }        
		return result;
	}
}
