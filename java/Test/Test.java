import com.xerox.jatanor.*;
import java.io.*;
import java.util.*;

/**
*  Description of the Class
*
* @author     roux
* @created    23 January 2017
*/

public class Test {
    int gHandler;
    JAtanor jatan;

    /**
    *  Description of the Method
    *
    * @param  grmFile  Description of the Parameter
    */
    public Test(String atanorfile) {
        try {
            System.out.println("Execute: " + atanorfile);
            // create a JAtanor object to load the dynamic library in the VM
            jatan = new JAtanor();
            int h=jatan.LoadProgram(atanorfile,"");
            System.out.println("Handler:"+h);
            String[] arg=new String[1];
            arg[0]="I call you";
            String res=jatan.ExecuteFunction(h, "rappel",arg);
            System.out.println("Result:"+res);

        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }



    /**
    *  Description of the Method
    *
    * @param  args  Description of the Parameter
    */
    public final static void main(String args[]) {
        try {
            Test test = new Test(args[0]);
            } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
}
