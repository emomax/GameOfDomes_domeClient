package debugger
{
	
	import flash.globalization.DateTimeFormatter;
	import flash.globalization.LocaleID;
	import utils.Formatting;
	
	public class Debug
	{	
		private static function getNow() : String {
			var d:Date = new Date();
			var dtf:DateTimeFormatter = new DateTimeFormatter(LocaleID.DEFAULT);
			dtf.setDateTimePattern("yy-MM-dd hh:mm:ss");
			return dtf.format(d);
		}
		
		public static function INFO(s:String, o:Object = null) : void {
			// if caller is specified, type it out
			trace("[DEBUG.LOG]: "+ getNow() + (o==null ? " " : " [" + Formatting.getName(o) + "]: ") + s); 
		}
		
		public static function WARNING(s:String) : void {
			trace("[DEBUG.WARNING]: "+ getNow() + ": " + s);
		}
		
		public static function formatArray(a:Array) : String {
			var s:String = "";
			for (var i:int = 0; i < a.length; ++i) {
				s += "[" + a[i] + "], ";
			}
			
			return s;
		}
	}
}