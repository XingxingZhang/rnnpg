package dio;

import static dio.EasyIO.*;
import java.io.*;

public class XProcess {
	
	public static void exec(String cmd)
	{
		println(cmd);
		Runtime run = Runtime.getRuntime();
		try{
			Process p = run.exec(cmd); 
            BufferedReader br = new BufferedReader(new InputStreamReader(new BufferedInputStream(p.getInputStream())));
            BufferedReader bre = new BufferedReader(new InputStreamReader(new BufferedInputStream(p.getErrorStream())));
            String line = null;  
            while ((line = br.readLine()) != null)   
                System.out.println(line);
            while ((line = bre.readLine()) != null)
            	System.err.println(line);
            if (p.waitFor() != 0) {
                if (p.exitValue() == 1)
                    System.err.println("command execution failed!");  
            }
            br.close();
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	public static void exec(String []cmd)
	{
		Runtime run = Runtime.getRuntime();
		try{
			Process p = run.exec(cmd); 
            BufferedReader br = new BufferedReader(new InputStreamReader(new BufferedInputStream(p.getInputStream())));
            BufferedReader bre = new BufferedReader(new InputStreamReader(new BufferedInputStream(p.getErrorStream())));
            String line = null;  
            while ((line = br.readLine()) != null)   
                System.out.println(line);
            while ((line = bre.readLine()) != null)
            	System.err.println(line);
            if (p.waitFor() != 0) {
                if (p.exitValue() == 1)
                    System.err.println("command execution failed!");  
            }
            br.close();
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public static void main(String args[])
	{
		exec("wc -l *");
		exec(new String[]{"ls", "-l"});
		println("done");
	}
}
