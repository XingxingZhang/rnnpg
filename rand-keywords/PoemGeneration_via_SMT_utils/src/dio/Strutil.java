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
	
}