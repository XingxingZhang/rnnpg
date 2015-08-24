package dio;

/**
 * Well, the function of this class is easy to see from its name
 * it's an interface for IO operations, but it's easier to use :)
 * @author zxx
 * version: 2011-05-21
 * version: 2011-12-27
 */


import java.io.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.*;

public class EasyIO {
	public static PrintWriter getpw( File file ) throws Exception
	{
		PrintWriter pw = new PrintWriter( new BufferedWriter( new FileWriter( file ) ) );
		return pw;
	}
	public static PrintWriter getpw( String fileName ) throws Exception
	{
		PrintWriter pw = new PrintWriter( new BufferedWriter( new FileWriter( fileName ) ) );
		return pw;
	}
	public static PrintWriter getpwa( String fileName ) throws Exception
	{
		PrintWriter pw = new PrintWriter( new BufferedWriter( new FileWriter( fileName, true ) ) );
		return pw;
	}
	public static PrintWriter getpw( String fileName, String encode ) throws Exception
	{
		PrintWriter pw = new PrintWriter(  
			    new OutputStreamWriter(  
			            new FileOutputStream(fileName),  
			            encode));  
		return pw;
	}
	public static PrintWriter getpwa( String fileName, String encode ) throws Exception
	{
		PrintWriter pw = new PrintWriter(  
			    new OutputStreamWriter(  
			            new FileOutputStream(fileName,true),  
			            encode));  
		return pw;
	} 
	public static PrintWriter getpw( File file, String encode ) throws Exception
	{
		PrintWriter pw = new PrintWriter(  
			    new OutputStreamWriter(  
			            new FileOutputStream(file),  
			            encode));  
		return pw;
	}
	public static PrintWriter getpwa( File file, String encode ) throws Exception
	{
		PrintWriter pw = new PrintWriter(  
			    new OutputStreamWriter(  
			            new FileOutputStream(file,true),  
			            encode));  
		return pw;
	}
	public static BufferedReader getbr( File file ) throws Exception
	{
		BufferedReader br = new BufferedReader( new FileReader( file ) );
		return br;
	}
	public static BufferedReader getbr( String fileName ) throws Exception
	{
		BufferedReader br = new BufferedReader( new FileReader( fileName ) );
		return br;
	}
	public static BufferedReader getbr( String fileName, String encode ) throws Exception
	{
		BufferedReader reader = new BufferedReader(new InputStreamReader(
                new FileInputStream(new File(fileName)), encode));
		return reader;
	}
	public static BufferedReader getbr( File file, String encode ) throws Exception
	{
		BufferedReader reader = new BufferedReader(new InputStreamReader(
                new FileInputStream(file), encode));
		return reader;
	}
	public static BufferedWriter getbw( String fileName, String encode ) throws Exception
	{
		return new BufferedWriter(new OutputStreamWriter(
                new FileOutputStream(new File(fileName)), encode));
	}
	public static PrintStream getps( String fileName ) throws Exception
	{
		PrintStream ps = new PrintStream( fileName );
		return ps;
	}
	public static PrintStream getps( File file ) throws Exception
	{
		PrintStream ps = new PrintStream( file );
		return ps;
	}
	public static void println( Object obj )
	{
		System.out.println( obj );
	}
	public static void println()
	{
		System.out.println();
	}
	public static void print( Object obj )
	{
		System.out.print( obj );
	}
	public static void printf( String format, Object... args )
	{
		System.out.printf( format, args );
	}
	public static String readAll( String fileName, String lineBreak )
	{
		BufferedReader br = null;
		StringBuffer sb = new StringBuffer();
		try{
			br = getbr( fileName );
			String line = "";
			while( (line = br.readLine()) != null )
				sb.append( line + lineBreak );
		}catch( Exception e )
		{
			e.printStackTrace();
		}
		return sb.toString();
	}
	public static String readAll( File file, String lineBreak )
	{
		BufferedReader br = null;
		StringBuffer sb = new StringBuffer();
		try{
			br = getbr( file );
			String line = "";
			while( (line = br.readLine()) != null )
				sb.append( line + lineBreak );
		}catch( Exception e )
		{
			e.printStackTrace();
		}
		return sb.toString();
	}
	public static String readAll( File file, String lineBreak, String encode )
	{
		BufferedReader br = null;
		StringBuffer sb = new StringBuffer();
		try{
			br = getbr( file, encode );
			String line = "";
			while( (line = br.readLine()) != null )
				sb.append( line + lineBreak );
		}catch( Exception e )
		{
			e.printStackTrace();
		}
		return sb.toString();
	}
	public static String readAll( String fileName, String lineBreak, String encode )
	{
		BufferedReader br = null;
		StringBuffer sb = new StringBuffer();
		try{
			br = getbr( fileName, encode );
			String line = "";
			while( (line = br.readLine()) != null )
				sb.append( line + lineBreak );
		}catch( Exception e )
		{
			e.printStackTrace();
		}
		return sb.toString();
	}
	public static String readAll( File file )
	{
		return readAll( file, "\n" );
	}
	public static String readAll( String fileName )
	{
		return readAll( fileName, "\n" );
	}
	public static String readAllC( File file, String encode )
	{
		return readAll( file, "\n", encode );
	}
	public static String readAllC( String fileName, String encode )
	{
		return readAll( fileName, "\n", encode );
	}
	public static DataInputStream getdis( String fileName ) throws Exception
	{
		return new DataInputStream( new BufferedInputStream( new FileInputStream( fileName ) ) );
	}
	public static DataInputStream getdis( File file ) throws Exception
	{
		return new DataInputStream( new BufferedInputStream( new FileInputStream( file ) ) );
	}
	public static DataOutputStream getdos( String fileName ) throws Exception 
	{
		return new DataOutputStream( new BufferedOutputStream( new FileOutputStream( fileName ) ) );
	}
	public static DataOutputStream getdos( File file ) throws Exception 
	{
		return new DataOutputStream( new BufferedOutputStream( new FileOutputStream( file ) ) );
	}
	public static void write2f( String fileName, String str )
	{
		try{
			PrintWriter pw = getpw(fileName);
			pw.print(str);
			pw.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static void write2f( String fileName, String str, String ENCODE )
	{
		try{
			PrintWriter pw = getpw(fileName, ENCODE);
			pw.print(str);
			pw.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static void write2f( File file, String str )
	{
		try{
			PrintWriter pw = getpw(file);
			pw.print(str);
			pw.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static void write2f( File file, String str, String ENCODE )
	{
		try{
			PrintWriter pw = getpw(file, ENCODE);
			pw.print(str);
			pw.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static String[] matchfirst( String regex, String input )
	{
		String []res = null;
		Pattern p = Pattern.compile(regex);
		Matcher m = p.matcher(input);
		if ( m.find() )
		{
			int n = m.groupCount();
			res = new String[n+1];
			res[0] = m.group();
			for( int i = 1; i <= n; i ++ )
				res[i] = m.group(i);
		}
		
		return res;
	}
	public static String matchfirst( String regex, String input, int i )
	{
		Pattern p = Pattern.compile(regex);
		Matcher m = p.matcher(input);
		if ( m.find() )
			return m.group(i);
		return null;
	}
	public static ArrayList<String[]> matchall( String regex, String input )
	{
		ArrayList<String[]> mats = new ArrayList<String[]>();
		Pattern p = Pattern.compile(regex);
		Matcher m = p.matcher(input);
		while( m.find() )
		{
			int n = m.groupCount();
			String []res = new String[n+1];
			res[0] = m.group();
			for( int i = 1; i <= n; i ++ )
				res[i] = m.group(i);
			mats.add(res);
		}
		return mats;
	}
	public static ArrayList<String> matchall( String regex, String input, int i )
	{
		ArrayList<String> mats = new ArrayList<String>();
		Pattern p = Pattern.compile(regex);
		Matcher m = p.matcher(input);
		while( m.find() )
			mats.add(m.group(i));
		
		return mats;
	}
	public static void tlparse( String fileName, Object obj, TL tl  )
	{
		try{
			BufferedReader br = getbr(fileName);
			String line = "";
			while( (line=br.readLine()) != null )
			{
				if ( (line=line.trim()).length() == 0 ) continue;
				tl.lparse( line, obj );
			}
			br.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static void tlfparse( String fileName, Object obj, String sep, TLF tlf )
	{
		try{
			BufferedReader br = getbr(fileName);
			String line = "";
			while( (line=br.readLine()) != null )
			{
				if ( (line=line.trim()).length() == 0 ) continue;
				String []fields = line.split( sep );
				tlf.fparse(fields, obj);
			}
			br.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static void tlfparse( String fileName, Object obj, TLF tlf )
	{
		tlfparse(fileName, obj, "\t", tlf);
	}
	public static int wc_l( String inFile )
	{
		int cnt = 0;
		try{
			BufferedReader br = getbr(inFile);
			while( br.readLine() != null )
				cnt ++;
			br.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
		return cnt;
	}
	public static void cp( String srcFile, String dstFile )
	{
		try{
			FileInputStream fis = new FileInputStream(srcFile);
			FileOutputStream fos = new FileOutputStream(dstFile);
			final int BUF_SIZE = 10240;
			byte []buf = new byte[BUF_SIZE];
			int size = -1;
			while( (size = fis.read(buf) ) != -1 )
				fos.write( buf, 0, size );
			fis.close();
			fos.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static void cp( File srcFile, File dstFile )
	{
		try{
			FileInputStream fis = new FileInputStream(srcFile);
			FileOutputStream fos = new FileOutputStream(dstFile);
			final int BUF_SIZE = 10240;
			byte []buf = new byte[BUF_SIZE];
			int size = -1;
			while( (size = fis.read(buf) ) != -1 )
				fos.write( buf, 0, size );
			fis.close();
			fos.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static void cpr( String srcFile, String dstDir )
	{
		cpr( new File(srcFile), dstDir );
	}
	public static void cpr( File srcFile, String dstDir )
	{
		if ( srcFile.isDirectory() )
		{
			File newDir = new File( dstDir + File.separator + srcFile.getName() );
			if ( !newDir.exists() ) newDir.mkdir();
			File []fList = srcFile.listFiles();
			for( File f : fList )
				cpr(f,dstDir + File.separator + srcFile.getName());
		}
		else
			cp(srcFile, new File(dstDir + File.separator + srcFile.getName()));
	}
	public static String tosafename( String fileName )
	{
		String newName = "";
		for( int i = 0; i < fileName.length(); i ++ )
		{
			char ch = fileName.charAt(i);
			if( Character.isLetter(ch) || Character.isDigit(ch) || ch == ' ' || ch == '_' )
				newName += ch;
			else
				newName += '-';
		}
		return newName;
	}
	public static void mkdir( String dir )
	{
		new File(dir).mkdir();
	}
	public static void mkdirs( String dir )
	{
		new File(dir).mkdirs();
	}
	public static String dfjoin(String dir, String fname)
	{
		dir = dir.endsWith( File.separator ) ? dir : dir + File.separator;
		return dir + fname;
	}
	public static String join(String sep, String []strs)
	{
		StringBuffer sb = new StringBuffer();
		sb.append(strs[0]);
		for( int i = 1; i < strs.length; i ++ )
			sb.append(sep + strs[i]);
		return sb.toString();
	}
	public static String join(String sep, List<String> strs)
	{
		StringBuffer sb = new StringBuffer();
		sb.append(strs.get(0));
		for( int i = 1; i < strs.size(); i ++ )
			sb.append(sep + strs.get(i));
		return sb.toString();
	}
	public static String join(String sep, String str)
	{
		StringBuffer sb = new StringBuffer();
		sb.append(str.charAt(0));
		for( int i = 1; i < str.length(); i ++ )
			sb.append(sep + str.charAt(i));
		return sb.toString();
	}
	public static void splitl(String infile, String outPre, int N)
	{
		int lineCount = wc_l(infile);
		int nDigits = Integer.toString(N-1).length();
		int each = lineCount / N;
		if(lineCount % N != 0)
			each ++;
		try{
			BufferedReader br = getbr(infile);
			PrintWriter []pw = new PrintWriter[N];
			int i;
			for( i = 0; i < N; i ++ )
				pw[i] = getpw(outPre + String.format("%0"+nDigits+"d", i));
			String line = null;
			int cnt = 0;
			while( (line=br.readLine()) != null )
			{
				pw[cnt/each].println(line);
				cnt ++;
			}
			br.close();
			for( i = 0; i < N; i ++ )
				pw[i].close();
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	public static void merged(String indir, String outfile, boolean sortedByName)
	{
		try{
			PrintWriter pw = getpw(outfile);
			File []fList = new File(indir).listFiles();
			if(sortedByName)
			{
				Arrays.sort(fList, new Comparator<File>(){

					@Override
					public int compare(File o1, File o2) {
						return o1.getName().compareTo(o2.getName());
					}
					
				});
			}
			for( File f : fList )
			{
				BufferedReader br = getbr(f);
				String line = null;
				while((line=br.readLine()) != null)
					pw.println(line);
				br.close();
			}
			pw.close();
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	public static void merged(String indir, String outfile)
	{
		merged(indir, outfile, false);
	}
	public static void merged(String indir, String regex, String outfile)
	{
		try{
			PrintWriter pw = getpw(outfile);
			File []fList = new File(indir).listFiles();
			for( File f : fList )
			{
				if(!f.getName().matches(regex))
					continue;
				BufferedReader br = getbr(f);
				String line = null;
				while((line=br.readLine()) != null)
					pw.println(line);
				br.close();
			}
			pw.close();
		}catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	public static void rm_r(File inFile)
	{
		if(inFile.isDirectory())
		{
			for(File f : inFile.listFiles())
				rm_r(f);
			inFile.delete();
		}
		else
			inFile.delete();
	}
	public static void rm_r(String infile)
	{
		rm_r(new File(infile));
	}
}
