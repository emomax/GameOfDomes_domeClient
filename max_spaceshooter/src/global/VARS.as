package global
{
	import flash.media.SoundTransform;
	
	public class VARS
	{
		public static const airRes:Number = 0.9;
		public static var showBoundingBoxes:Boolean = false;
		public static var currProgress:int = -1;
		public static var storyProgress:int = 0;
		public static var levelProgress:int = 3;
		
		public static var soundVolume:SoundTransform = new SoundTransform(0.2);
	}
}