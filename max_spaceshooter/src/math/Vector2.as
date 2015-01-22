package math {
	
	import flash.geom.Point;
	
	public class Vector2 {
		
		/** 
		 * Returns the dot product of given vectors u & v
		 * 
		 * Formula: u*v = |u||v|cos(theta)
		 */
		
		public static function dot (u:Point, v:Point) : Number {
			return Math.acos((u.x * v.x + u.y * v.y) / (abs(u)*abs(v)));
		}
		
		/**
		 * Returns the absolute value of given vector v
		 */
		public static function abs (v:Point) : Number { 
			return Math.sqrt(v.x * v.x + v.y * v.y);
		}
		
		public static function format(u:Point) : String {
			return "(" + u.x + ", " + u.y + ")";
		}
	}
}