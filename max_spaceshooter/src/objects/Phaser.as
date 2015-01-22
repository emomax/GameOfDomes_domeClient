package objects
{
	import flash.events.TimerEvent;
	import flash.geom.Point;
	import flash.text.engine.BreakOpportunity;
	import flash.utils.Timer;
	
	import debugger.Debug;
	
	import global.VARS;
	
	import objects.AI;
	
	import starling.display.Image;
	import starling.display.Sprite;
	import starling.events.Event;
	
	public class Phaser extends Ship implements PrimaryWeapon, AI, STALKER, ENEMY
	{
		private var art:Image = new Image(Assets.getAtlas().getTexture("phaser"));
		
		public var _playerRef:Ship;
		
		private var _fireSpeed:Number;
		private var _fireTimer:Timer;
		private var _firePower:int
		private var _canFire:Boolean;
		
		private var disableTimer:Timer;
		private var _state:String = "enters";
		private var fireIterator:uint = 0;
		
		public function Phaser(s:Sprite, p:Ship) {
			super(s);
			this._HP = 100;
			speed = 0.4;
			
			_playerRef = p;
			
			// PHASER SPECIFIC ATTRIBUTES
			fireSpeedPrimary = 600;
			canFirePrimary = true;
			firePowerPrimary = 21;
			fireTimerPrimary = new Timer(fireSpeedPrimary, 1);
			
			fireTimerPrimary.addEventListener(TimerEvent.TIMER, function(e:TimerEvent) : void { fireTimerPrimary.stop(); canFirePrimary = true; });
			
			disableTimer = new Timer(1000);
			disableTimer.addEventListener(TimerEvent.TIMER, function (e:TimerEvent) : void {
				disableTimer.stop();
				state = "combat";
			});
		}
		
		override protected function onAddedToStage(e:Event) : void {
			this.createExhaustArt(new Point(-10, -15), -1);
			this.createExhaustArt(new Point(-10, -5), -1);
			this.createExhaustArt(new Point(-10, 5), -1);
			this.createShipArt("phaser");
			
			this.boundingbox = this.bounds;
			this.showBoundingBox();
			direction = 1;
		}
		
		override public function init() : void {
			// Init the Phaser!
			Debug.INFO("I am initiated!", this);
			addEventListener(Event.ENTER_FRAME, behaviourLoop);
		}
		
		public function behaviourLoop(e:Event) : void {
			// Do stuff;
			
			switch (state) {
				case "enters":
					if (this.x > shipArt.width) {
						state = "combat";
					} else {
						this.x += speed * 2;
					}
					
					break;
				case "combat":
					if (!_playerRef) {
					this.removeEventListener(Event.ENTER_FRAME, behaviourLoop);
						return;
					}
					copyPlayerMoves();
					
					velY *= VARS.airRes; 
					
					if(Math.round(velY*5) == 0) velY = 0;
					
					this.y += velY;
				break;
				
				case "idle":
					break;
				default:
					throw new Error("Invalid state for Phaser: " + state);
			}
		}
		
		public function copyPlayerMoves() : void
		{	
			if(Math.abs(_playerRef.y + Math.round(_playerRef.getWeaponDistance().y) - this.y) > 5) {
				this.velY = (_playerRef.y + Math.round(_playerRef.getWeaponDistance().y) < this.y) ? velY - speed : velY + speed;
			}
			else {
				primary();
			}
		}
		
		override public function primary() : void {
			addEventListener(Event.ENTER_FRAME, fireBurst);
			state = "idle";
			fireIterator = 0;
			return;
			
			if (canFirePrimary) {
				// Weapon can fire! Let's do it. FIRE() !!
				fireTimerPrimary.start();
				canFirePrimary = false;
				fire();
			}
			else {
				// Weapon on cooldown. No firing allowed.
			}
		}
		
		private function fireBurst(e:Event) : void {
			if (fireIterator > 20) {
				this.removeEventListener(Event.ENTER_FRAME, fireBurst);
				disableTimer.start();
				fireIterator = 0;
			}
			
			if (fireIterator++ == 0 || fireIterator == 15 || fireIterator == 30)
				fire();
		}
		
		public function fire() : void {
			var m:MuzzleFlash = new MuzzleFlash(stageRef);
			var l:Laser = new Laser(stageRef, this, 1, firePowerPrimary);
			l.x = this.x - 30;
			l.y = this.y - 15;
			
			m.x = this.x + 21;
			m.y = this.y - 7;
			
			stageRef.addChild(m);
			stageRef.addChild(l);
		}
		
		// Interface PrimaryWeapon GETTERS
		
		public function get fireSpeedPrimary():Number { return _fireSpeed; }
		public function get fireTimerPrimary():Timer { return _fireTimer; }
		public function get canFirePrimary():Boolean { return _canFire; }
		public function get firePowerPrimary():int { return _firePower; }
		
		// Interface PrimaryWeapon SETTERS
		public function set fireSpeedPrimary(n:Number):void { _fireSpeed = n; }
		public function set fireTimerPrimary(t:Timer):void { _fireTimer = t; }
		public function set canFirePrimary(b:Boolean):void { _canFire = b; }
		public function set firePowerPrimary(i:int):void { _firePower = i; }
		
		// Interface ENEMY
		public function get target() : Ship { return _playerRef; }
		public function set target(s:Ship) : void { _playerRef = s }
		
		public function get state() : String { return _state;}
		public function set state(s:String) : void { 
			Debug.INFO("Phaser state is now: " + _state, this);
			_state = s; 
		}
		
		public function clean() : void {
			removeEventListener(Event.ENTER_FRAME, behaviourLoop);
			_playerRef = null;
		}
	}
}