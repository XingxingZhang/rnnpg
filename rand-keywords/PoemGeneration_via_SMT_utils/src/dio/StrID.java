package dio;

import java.io.BufferedReader;
import java.io.PrintWriter;
import java.util.*;
import static dio.EasyIO.*;

public class StrID {
	
	private HashMap<String,Integer> str2id = new HashMap<String,Integer>();
	private ArrayList<String> id2str = new ArrayList<String>();
	private int start_id;
	
	public StrID( int start_id )
	{
		this.start_id = start_id;
	}
	public StrID()
	{
		this(0);
	}
	/**
	 * add an entry, and return its id
	 * @param str
	 * @return
	 */
	public int add( String str )
	{
		Integer id = str2id.get(str);
		if( id == null )
		{
			int new_id = str2id.size();
			str2id.put(str, new_id );
			id2str.add(str);
			return new_id + start_id;
		}
		else
			return id + start_id;
	}
	/**
	 * get the id of str, return -1 if str does not exist
	 * @param str
	 * @return
	 */
	public int getID( String str )
	{
		Integer id = str2id.get(str);
		if( id != null )
			return id + start_id;
		else
			return -1;
	}
	/**
	 * get str according to id
	 * @param id
	 * @return
	 */
	public String getStr( int id )
	{
		id -= start_id;
		if( id < 0 || id >= id2str.size() )
			return null;
		else
			return id2str.get(id);
	}
	public int size()
	{
		return id2str.size();
	}
	public void save( String outfile )
	{
		try{
			PrintWriter pw = getpw(outfile);
			pw.println( size() );
			pw.println(this.start_id);
			for( String str : this.id2str )
				pw.println(str);
			pw.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	public void load( String infile )
	{
		try{
			BufferedReader br = getbr(infile);
			int size = Integer.parseInt(br.readLine());
			this.start_id = Integer.parseInt(br.readLine());
			for( int i = 0; i < size; i ++ )
			{
				String str = br.readLine();
				add(str);
			}
			br.close();
		}catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	/*
	public static void main( String args[] )
	{
		StrID strid = new StrID(1);
		String str = "asdfgjksldfjdskladfjasklfjk23804uwermx";
		for( int i = 0; i < str.length(); i ++ )
		{
			String ch = Character.toString(str.charAt(i));
			strid.add(ch);
		}
		strid.save( "dict.txt" );
		println( strid.getID( "g" ) );
		println( strid.getStr( 5 ) );
		strid = new StrID();
		strid.load("dict.txt");
		println( strid.getID( "g" ) );
		println( strid.getStr( 5 ) );
		strid.save("dict1.txt");
	}
	*/
}
