package dio;

import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

public class FreqSampler {
	public static final int NORMAL = 0;
	public static final int ASCENDING = 1;
	public static final int DESCENDING= 2;
	public HashMap<String,ArrayList<String>> str_sample = new HashMap<String,ArrayList<String>>();
	ArrayList<Map.Entry<String,ArrayList<String>>> list = new ArrayList<Map.Entry<String,ArrayList<String>>>();
	
	public void add( String str, String val )
	{
		ArrayList<String> sample = str_sample.get(str);
		if( sample == null )
		{
			sample = new ArrayList<String>();
			sample.add(val);
			str_sample.put(str, sample);
		}
		else
			sample.add(val);
	}
	public ArrayList<String> get( String str )
	{
		ArrayList<String> vals = str_sample.get(str);
		return vals;
	}
	public int size()
	{
		return this.str_sample.size();
	}
	void sort( int _mode )
	{
		final int mode = _mode;
		for( Map.Entry<String, ArrayList<String>> entry : str_sample.entrySet() )
			list.add( entry );
		Collections.sort(list,new Comparator<Map.Entry<String, ArrayList<String>>>(){

			@Override
			public int compare(Entry<String, ArrayList<String>> arg0,
					Entry<String, ArrayList<String>> arg1) {
				if( mode == FreqSampler.DESCENDING )
					return arg1.getValue().size() - arg0.getValue().size();
				else if( mode == FreqSampler.ASCENDING )
					return arg0.getValue().size() - arg1.getValue().size();
				else
					return 0;
			}
			
		});
	}
	void print( PrintWriter pw, Map.Entry<String, ArrayList<String>> entry )
	{
		pw.println(entry.getKey());
		ArrayList<String> vals = entry.getValue();
		for( String str : vals )
			pw.println(str);
		pw.println( "============================================================================" );
		pw.println();
		pw.println();
	}
	void print( PrintWriter pw, int mode )
	{
		if( mode != FreqSampler.NORMAL )
		{
			sort(mode);
			for( Map.Entry<String, ArrayList<String>> entry : list )
				print(pw,entry);
		}
		else
		{
			for( Map.Entry<String, ArrayList<String>> entry : str_sample.entrySet() )
				print(pw,entry);
		}
	}
}
