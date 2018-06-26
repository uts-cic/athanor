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
public class Atangrammar extends Thread {
	Atanorgrm grm;
	String[] result;
	String json;

    /**
    *  Description of the Method
    *
    * @param  grmFile  Description of the Parameter
    */
    public Atangrammar(Atanorgrm a) {
		grm=a;
    }

    //Needed to apply the grammar to the JSON vector...
    public void run() {
        try {
            result=grm.Execute(json);
        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
            result=new String[1];
        }        
		
		Display();
    }

    //We just need this method to load our JSON structure to exemplify our library
    void readFile(String path) throws IOException {
        byte[] encoded = Files.readAllBytes(Paths.get(path));
        json = new String(encoded, StandardCharsets.UTF_8);
    }

	void Display() {
		System.out.println();
		for (int i=0; i< result.length; i++)
			System.out.println(result[i]);
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
								      
			//we load a file  that contains a typical JSON structure
            
			Atanorgrm grm1=new Atanorgrm();
			grm1.Load(args[0]);

			Atanorgrm grm2=new Atanorgrm();
			grm2.Load(args[1]);
			
			
            Atangrammar test1 = new Atangrammar(grm1);			
            test1.readFile(args[2]);

			Atangrammar test11 = new Atangrammar(grm1);			
            test11.readFile(args[2]);
			
			Atangrammar test2 = new Atangrammar(grm2);
            test2.readFile(args[2]);

            Atangrammar test3 = new Atangrammar(grm1);			
            test3.readFile(args[2]);
			
			Atangrammar test4 = new Atangrammar(grm2);
            test4.readFile(args[2]);

			
			test1.start();			
			test11.start();
			test2.start();
			test3.start();
			test4.start();

        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
}

