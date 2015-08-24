package rnn_poem;

import static dio.EasyIO.*;
import java.io.*;

import dio.StrID;

public class Postprocessor {
	
	String FILE_ENCODE = "utf-8";
	
	String tostdkeywords(String keystr)
	{
		keystr = keystr.trim();
		String []fields = keystr.split( "\\*" );
		String outstr = "";
		for(String field : fields)
		{
			field = field.trim().replaceAll("\\s+", "");
			outstr += field + " ";
		}
		
		return outstr.trim();
	}
	
	// format 
	// id \t keywords \t senLen = 5
	// line1
	// line2
	// line3
	// line4
	void rnnOutformat2stdformat(String infile, String outfile)
	{
		try{
			StrID key2id = new StrID(1);
			BufferedReader br = getbr(infile, this.FILE_ENCODE);
			PrintWriter pw = getpw(outfile, this.FILE_ENCODE);
			String line = null;
			String poem = "";
			while((line=br.readLine()) != null)
			{
				if((line=line.trim()).length() == 0) continue;
				if(line.startsWith("<"))
				{
					if(poem.length() != 0)
					{
						pw.println(poem);
						poem = "";
					}
					
					int pos1 = line.indexOf("=");
					int pos2 = line.lastIndexOf("*");
					int pos3 = line.lastIndexOf("\t");
					if(pos3 == -1)
						pos3 = line.length();
					println(pos1 + "\t" + pos2 + "\t" + pos3);
					println(line);
					String keystr = line.substring(pos1 + 1, pos2);
					String keywords = tostdkeywords(keystr);
					String senlenstr = line.substring(pos2 + 1, pos3).trim();
					int id = key2id.add(keywords);
					pw.println(id + "\t" + keywords + "\t" + senlenstr);
				}
				else if(!line.contains("|||"))
					poem = poem + line.replace(" ", "") + "\n";
			}
			
			if(poem.length() != 0)
			{
				pw.println(poem);
				poem = "";
			}
			
			br.close();
			pw.close();
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public static void main(String []args)
	{
		if(args.length != 2)
			printeln("post-processing <rnn_out_poem_file> <std_poem_file>");
		
		Postprocessor post = new Postprocessor();
		post.rnnOutformat2stdformat(args[0], args[1]);
	}
}
