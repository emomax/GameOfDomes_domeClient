package utils
{
	import flash.utils.getQualifiedClassName;
	
	public class Formatting
	{
		public static function getName(o:Object) : String {
			return getQualifiedClassName(o).replace(/.+::/, ""); // returns the class of the caller in text.
		}
		
		public static function formatArray(a:Array) : String {
			var s:String = "[ ";
			trace( "array.length is: " + a.length);
			
			for (var i:uint = 0; i < a.length; ++i) {
				s += getName(a[i]) + ", ";
			}
			
			s = s.substring(0, s.length - 2) + "]";
			
			return s;
		}
	}
}