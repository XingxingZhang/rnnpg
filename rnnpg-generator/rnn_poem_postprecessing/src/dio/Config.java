package dio;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.Map;
import java.util.Properties;

public class Config {
	/**
	 * the rules for the path naming:
	 * xxxF means the path of a file
	 * xxxD means the path of a directory
	 */
	
	public static Properties prop = new Properties();
	private static String root_path = null;
	
	public static String getRPath( String fName )
	{
		if( root_path == null )
			root_path = get("rootD");
		return root_path + File.separator + get(fName);
	}
	public static int getInt( String key )
	{
		return Integer.parseInt(get(key));
	}
	public static double getDouble( String key )
	{
		return Double.parseDouble(get(key));
	}
	public static boolean getBoolean( String key )
	{
		return Boolean.parseBoolean(get(key));
	}
	public static void add( String key, String val )
	{
		prop.put(key, val);
	}
	public static String get(String key)
	{
		return prop.getProperty(key);
	}
	
	public static void load( String infile )
	{
		try{
			prop.load( new FileInputStream(infile) );
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static void save( String outfile )
	{
		try{
			prop.store(new FileOutputStream(outfile), null);
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public static void print()
	{
		System.out.println( "=============================================" );
		for( Map.Entry<Object, Object> entry : prop.entrySet() )
			System.out.println( entry.getKey() + "\t" + entry.getValue() );
		System.out.println( "=============================================" );
	}
	public static void main( String args[] )
	{
		Config.load("poem-gen.conf");
		Config.print();
	}
}
