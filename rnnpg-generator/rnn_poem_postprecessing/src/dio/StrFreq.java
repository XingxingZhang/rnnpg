package dio;

import java.util.*;

public class StrFreq {
	
	private HashMap<String,Integer> str_freq = new HashMap<String,Integer>();
	
	public void add( String str )
	{
		Integer freq = str_freq.get(str);
		str_freq.put(str, freq == null? 1 : freq + 1);
	}
	public void add( Collection<String> strs )
	{
		for( String str : strs )
			add(str);
	}
	public Integer get( String str )
	{
		return str_freq.get(str);
	}
	public boolean contrains( String str )
	{
		return str_freq.containsKey(str);
	}
	public boolean freqGT( String str, int freq )
	{
		Integer freq_ = str_freq.get(str);
		if(freq_ == null)
			return false;
		else
			return freq_ > freq;
	}
	public boolean freqGE( String str, int freq )
	{
		Integer freq_ = str_freq.get(str);
		if(freq_ == null)
			return false;
		else
			return freq_ >= freq;
	}
}
