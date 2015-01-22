package objects
{
	
	import flash.media.Sound;
	
	import debugger.Debug;
	
	import starling.display.Sprite;
	import starling.events.Event;
	import starling.utils.deg2rad;
	
	public class Target extends Ship {
		
		private var startX:Number;
		private var startY:Number;
		private var iterator:int = 0;
		
		private var explosionSound:Sound = new Assets.ExplosionSound();
		
		public function Target(s:Sprite) : void {
			super(s);
		}
		
		override protected function onAddedToStage(e:Event):void {
			//this.createExhaustArt(new Point(-10, -10), -1);
			//this.createExhaustArt(new Point(-10, -5), -1);
			//this.createExhaustArt(new Point(-10, 0), -1);
			this.createShipArt("target");
			
			this._boundingbox = this.bounds;
			
			this.showBoundingBox();
		}
		
		public function fixatePosition() : void {
			this.startX = this.x;
			this.startY = this.y;
		}
		
		override public function init() : void {
			// Init the Phaser!
			Debug.INFO("I am initiated!", this);
			addEventListener(Event.ENTER_FRAME, behaviourLoop);
		}
		
		public function behaviourLoop(e:Event) : void {
			// Do stuff;
			this.y = startY + 10 * Math.sin(deg2rad(iterator += 3));
		}
	}
}