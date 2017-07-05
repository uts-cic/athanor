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

public class Atangrammar {
    int gHandler;
    JAtanor jatan;
	int handler;

    /**
    *  Description of the Method
    *
    * @param  grmFile  Description of the Parameter
    */
    public Atangrammar(String atanorfile) {
        try {
            // create a JAtanor object to load the dynamic library in the VM
            jatan = new JAtanor();
            handler=jatan.LoadProgram(atanorfile,"");
        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }

	//Needed to apply the grammar to the JSON vector...
    public String Apply(String json) {
        try {
            // create a JAtanor object to load the dynamic library in the VM
			String[] arg=new String[1];
            arg[0]=json;
            String res=jatan.ExecuteFunction(handler, "Apply",arg);
			return res;

        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
		return "";
    }

	//We just need this method to load our JSON structure to exemplify our library
	static String readFile(String path) throws IOException {
		byte[] encoded = Files.readAllBytes(Paths.get(path));
		return new String(encoded, StandardCharsets.UTF_8);
	}

    /**
    *  Description of the Method
    *
    * @param  args  Description of the Parameter
    */
    public final static void main(String args[]) {
        try {
			//We create an Atangrammar object, which will load a Athanor program (here it is reload.kif)
            Atangrammar test = new Atangrammar(args[0]);
			//we load a file  that contains a typical JSON structure
			String json=readFile("sentence.json");
			//We apply our grammar to it, which returns a String as value (which should be a JSON dictionary)
			String res= test.Apply(json);
			System.out.println(res);
            } 
			catch (Exception ex) {
				System.out.println(ex);
				ex.printStackTrace();
        }
    }
}

