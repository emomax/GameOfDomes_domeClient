package objects
{
	import flash.geom.Point;
	import flash.geom.Rectangle;
	
	import screens.InGame;
	import screens.Level;
	
	import starling.display.Image;
	import starling.display.Sprite;
	import starling.events.Event;
	
	public class Laser extends Sprite
	{
		private var art:Image;
		private var stageRef:Sprite;
		public var velocity:Point;
		public var dir:int;
		private var scatterY:Number = (Math.random() > 0.5) ? Math.round(Math.random() * 0.8) : -Math.round(Math.random() * 0.8);
		private var speed:int = 15;
		private var dmg:Number;
		
		public var hitBox:Rectangle;
		
		public var gunman:Ship;
		
		private var yDiff:Number;
		
		public var bigBlast:Boolean;
		
		public function Laser(s:Sprite, _gunman:Ship, dir:int = -1, _dmg:int = 10, vel:Point = null, bigblast:Boolean = false) : void
		{
			this.dir = dir;
			this.gunman = _gunman;
			this.dmg = _dmg;
			this.bigBlast = bigblast;
			
			if (vel == null) {
				velocity = new Point();
				velocity.x = speed * dir; 
				velocity.y = 0;
			} else {
				velocity = vel;
				velocity.x *= speed;
				velocity.y *= speed;
				scatterY = 0;
			}
			
			super();
			
			
			art = new Image(Assets.getAtlas().getTexture("laser_1"));
			stageRef = s;
			(stageRef as Level).bullets.push(this);
			
			addChild(art);
			
			hitBox = this.bounds;
			
			// Uncomment this to see bounding box
			//addChild(new DrawRect(hitBox));
			addEventListener(Event.ENTER_FRAME, laserLoop);
		}
		
		private function laserLoop (e:Event) : void {
			this.x += velocity.x;
			this.y += velocity.y + scatterY;
			
			if (this.x < -this.width || this.x > 700) {
				this.destroy();
			}
		}
		
		public function get power() : int { return dmg; }
		
		public function destroy() : void {
			if (stageRef.contains((stageRef as Level).bullets[(stageRef as Level).bullets.indexOf(this)])) {
				(stageRef as Level).bullets.splice((stageRef as Level).bullets.indexOf(this), 1);
				
				removeEventListener(Event.ENTER_FRAME, laserLoop);
				stageRef.removeChild(this, true);
				//trace("This bullet exists in bullets[]! Remove me!");
			}
			
			//trace("Destroyed laser");
		}
	}
}