package dio;

public class Strutil {
	/*
	public static boolean isAscii( String str )
	{
		// [\x00-\x7F]
		for( int i = 0; i < str.length(); i ++ )
		{
			char ch = str.charAt(i);
			if( !( ch >= 0x00 && ch <= 0x7F ) )
				return false;
		}
		return true;
	}
	*/
	public static boolean isAscii( String str )
	{
		// [\x00-\x7F]
		for( int i = 0; i < str.length(); i ++ )
		{
			char ch = str.charAt(i);
			if( !( ch >= 0x21 && ch <= 0x7E || " \t\n\f\r".indexOf(ch) != -1 ) )
				return false;
		}
		return true;
	}
	
	public static String subInd(String s, int index, String rep)
	{
		StringBuffer sb = new StringBuffer();
		int repLen = rep.length(), i;
		for(i = 0; i < index; i ++)
			sb.append(s.charAt(i));
		sb.append(rep);
		for(i = repLen + index; i < s.length(); i ++)
			sb.append(s.charAt(i));
		return sb.toString();
	}
}