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
    int hreflexive;
    int hanalytic;

    /**
    *  Description of the Method
    *
    * @param  grmFile  Description of the Parameter
    */
    public Atangrammar(String reflexive, String analytic) {
        try {
            // create a JAtanor object to load the dynamic library in the VM
            hreflexive=0;
            hanalytic=0;
            jatan = new JAtanor();
            hreflexive=jatan.LoadProgram(reflexive,"");
            hanalytic=jatan.LoadProgram(analytic,"");
        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }

    //Needed to apply the grammar to the JSON vector...
    public String[] Applyreflexive(String json) {
        String[] res;
        try {
            // create a JAtanor object to load the dynamic library in the VM
            String[] arg=new String[1];
            arg[0]=json;
            res=jatan.ExecuteFunctionArray(hreflexive, "Apply",arg);

        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
            res=new String[1];
        }
        return res;
    }

    //Needed to apply the grammar to the JSON vector...
    public String[] Applyanalytic(String json) {
        String[] res;
        try {
            // create a JAtanor object to load the dynamic library in the VM
            String[] arg=new String[1];
            arg[0]=json;
            res=jatan.ExecuteFunctionArray(hanalytic, "Apply",arg);

        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
            res=new String[1];
        }
        return res;
    }

    public void CleanAll() {
        try {
            jatan.CleanAll();
        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
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
            //We create an Atangrammar object, which will load a Athanor program
            System.out.println(args[0]);
            System.out.println(args[1]);
            System.out.println(args[2]);
            Atangrammar test = new Atangrammar(args[0], args[1]);
            //we load a file  that contains a typical JSON structure
            String json=readFile(args[2]);
            //We apply our grammar to it, which returns a String as value (which should be a JSON dictionary)
            System.out.println("Reflexive");
            String[] res= test.Applyreflexive(json);
            for (int i=0; i< res.length; i++)
                System.out.println(res[i]);
            System.out.println("Analytic");
            res= test.Applyanalytic(json);
            for (int i=0; i< res.length; i++)
                System.out.println(res[i]);
            test.CleanAll();
        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
}

