package objects {
	import com.freeactionscript.effects.explosion.AbstractExplosion;
	import com.freeactionscript.effects.explosion.LargeExplosion;
	
	import debugger.Debug;
	
	import screens.InGame;
	import screens.Level;
	
	import starling.core.Starling;
	import starling.display.MovieClip;
	import starling.display.Sprite;
	import starling.events.Event;
	import starling.utils.deg2rad;
	
	import utils.Formatting;
	
	public class Drone extends Ship {
		
		// add Hint 
		private var hint:MovieClip;
		
		// addIterator;
		private var iterator:uint;
		
		// does it count?
		private var _countsTowardsObjective:Boolean;
		
		public function Drone(s:Sprite, countsTowardsObjective:Boolean = true) {
			_countsTowardsObjective = countsTowardsObjective;
			super(s);
			Debug.INFO("A drone was created!", this);
			this.createShipArt("phaser");
		}
		
		public function setDirection(goingUp:Boolean) : void {
			Debug.INFO("setDirection() - was called, goingUp = " + goingUp, this);
			
			this.speed = goingUp ? -1 : 1;
			this.velY = 0;
			this._hpBar.visible = false;
			removeChild(this._hpBar);
			
			shipArt.rotation = deg2rad(goingUp ? -90 : 90);
			addEventListener(Event.ENTER_FRAME, droneLoop);
			iterator = 0;
			
			hint = new MovieClip(Assets.getAtlas().getTextures("spark_"), 15);
			hint.alignPivot();
			hint.scaleX = 0.5;
			hint.scaleY = 0.5;
			hint.y = 40 * speed;
			Starling.juggler.add(hint);
			
			addChild(hint);
		}
		
		private function droneLoop (e:Event) : void {
			
			if (iterator++ == 20 ) {
				hint.visible = false;
				return;
			} else if (iterator < 50){
				return;
			}
			
			this.velY += speed * 0.2;
			this.y += velY;
			
			if (velY < 0) {
				if (this.y < - shipArt.height / 2) {
					this.destroy();
				}
			} else {
				if (this.y > 550 + shipArt.height / 2) {
					this.destroy();
				}
			}
		}
		
		public function explode() : void {
			Debug.INFO("I explode!", this);
			destroy();
			
			var expl:AbstractExplosion = new LargeExplosion(stageRef);
			expl.create(this.x - shipArt.width / 2, this.y);
			explosionSound.play();
		}
		
		override protected function destroy() : void {
			Debug.INFO("Destroying, removing from stage and stops droneLoop", this);
			this.removeEventListener(Event.ENTER_FRAME, droneLoop);
			(stageRef as Level).ships.splice((stageRef as Level).ships.indexOf(this), 1);
			stageRef.removeChild(this, true);
			if (_countsTowardsObjective) 
				stageRef.dispatchEvent(new Event("enemyDown"));
		}
	}
}