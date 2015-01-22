package controllers {
	import flash.ui.Keyboard;
	
	import screens.Level;
	
	import starling.display.Sprite;
	import starling.events.KeyboardEvent;

	public class KeyboardHandler {
		private var stageRef:Sprite;
		
		private var LEFT:Boolean = false;
		private var RIGHT:Boolean = false;
		private var UP:Boolean = false;
		private var DOWN:Boolean = false;
		private var PAUSE:Boolean = false;
		
		private var FIRE1:Boolean = false;
		private var FIRE2:Boolean = false;
		
		public function KeyboardHandler(stageRef:Sprite) : void {
			if (stageRef == null) throw new Error();
			
			this.stageRef = stageRef;
			stageRef.addEventListener(KeyboardEvent.KEY_DOWN, downHandler);
			stageRef.addEventListener(KeyboardEvent.KEY_UP, upHandler);
		}
		
		private function downHandler (e:KeyboardEvent) : void {
			switch(e.keyCode) {
				case Keyboard.LEFT: 				// fall through
				case 65:	left 	= true; 	break;	// D pressed
				case Keyboard.RIGHT: 				// fall through
				case 68:	right 	= true; 	break;	// A pressed
				case Keyboard.DOWN: 				// fall through
				case 83:	down 	= true; 	break;	// S pressed
				case Keyboard.UP: 					// fall through
				case 87:	up 		= true; 	break;	// W pressed
				case 80:	break;//(stageRef as Level).showBoxes();	break;	// P pressed
				case Keyboard.SPACE:	fire1 	= true; 	break; 	// SPACE pressed
				case 71:	fire2 	= true; 	break; 	// G pressed
				case 78:	break;//(stageRef as Level).next();	break;	// N pressed
				default: 						break;
			}
		}
		
		private function upHandler (e:KeyboardEvent) : void {
			switch(e.keyCode) {
				case Keyboard.LEFT: 				// fall through
				case 65:	left 	= false; 	break;	// D released
				case Keyboard.RIGHT: 				// fall through
				case 68:	right 	= false; 	break;	// A released
				case Keyboard.DOWN: 				// fall through
				case 83:	down 	= false; 	break;	// S released
				case Keyboard.UP: 					// fall through
				case 87:	up 		= false; 	break;	// W released
				case 80:						break;	// P released
				case Keyboard.SPACE:	fire1 	= false; 	break; 	// SPACE released
				case 71:	fire2 	= false; 	break; 	// G released
				default: 						break;
			}
		}
		
		// GETTERS
		
		public function get up() : Boolean {
			return UP;
		}
		
		public function get down() : Boolean {
			return DOWN;
		}
		
		public function get right() : Boolean {
			return RIGHT;
		}
		
		public function get left() : Boolean {
			return LEFT;
		}
		
		public function get fire1() : Boolean {
			return FIRE1;
		}
		
		public function get fire2() : Boolean {
			return FIRE2;
		}
		
		// SETTERS
		
		public function set up(b:Boolean) : void {
			UP = b;
		}
		
		public function set down(b:Boolean) : void {
			DOWN = b;
		}
		
		public function set right(b:Boolean) : void {
			RIGHT = b;
		}
		
		public function set left(b:Boolean) : void {
			LEFT = b;
		}
		
		public function set fire1(b:Boolean) : void {
			FIRE1 = b;
		}
		
		public function set fire2(b:Boolean) : void {
			FIRE2 = b;
		}		
	}
}