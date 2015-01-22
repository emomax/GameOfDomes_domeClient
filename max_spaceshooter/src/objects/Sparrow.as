package objects
{
	import flash.events.TimerEvent;
	import flash.geom.Point;
	import flash.utils.Timer;
	
	import global.VARS;
	
	import objects.MuzzleFlash;
	
	import starling.display.MovieClip;
	import starling.display.Sprite;
	import starling.events.Event;
	
	public class Sparrow extends Ship implements PrimaryWeapon, SecondaryWeapon
	{
		private var _canFirePrimary:Boolean;
		private var _canFireSecondary:Boolean;
		
		private var _fireTimerPrimary:Timer;
		private var _fireTimerSecondary:Timer;
		
		private var _fireSpeedPrimary:Number;
		private var _fireSpeedSecondary:Number;
		
		private var _firePowerPrimary:int;
		private var _firePowerSecondary:int;
		
		private var _exhaustArt:MovieClip;
		
		
		
		public function Sparrow(stageRef:Sprite,  HP:int = 100, firePower:int = 12, fireSpeed:Number = 100) : void
		{
			super(stageRef);
			
			direction = -1;
			
			_HP = HP;
			
			_maxHp = _HP;
			
			// TEMPORARY SOLUTION. SEND WEAPONS AS OBJECTS() AS OPTIONAL PARAMETERS INSTEAD.
			firePowerPrimary = firePower;
			fireSpeedPrimary = fireSpeed;
			
			firePowerSecondary = firePower;
			fireSpeedSecondary = fireSpeed * 10;
			
			fireTimerPrimary = new Timer(fireSpeedPrimary, 1);
			fireTimerSecondary = new Timer(fireSpeedSecondary, 1);
			
			canFirePrimary = true;
			canFireSecondary = true;
			
			_weaponDistance = new Point(-9, 33);
		}
		
		override protected function onAddedToStage(e:Event):void {
			
			
			this.createShipArt("sparrow");
			_exhaustArt = this.createExhaustArt(new Point(45, 10));
			
			this._boundingbox = this.bounds;
			
			this.showBoundingBox();
			
			fireTimerPrimary.addEventListener(TimerEvent.TIMER, function(e:TimerEvent) : void { fireTimerPrimary.stop(); canFirePrimary = true; });
			fireTimerSecondary.addEventListener(TimerEvent.TIMER, function(e:TimerEvent) : void { fireTimerSecondary.stop(); canFireSecondary = true; });
			
			addHpBar();
			
			addEventListener(Event.ENTER_FRAME, sparrowLoop);
		}
		
		
		private function sparrowLoop(e:Event) : void {
			if (this._HP < this._maxHp) {
				this._HP += 0.05;
				if (_hpBar) {
					_hpBar.bar.scaleX = _HP / _maxHp;
				}
			}
		}
		
		public function get exhaustArt() : MovieClip { return _exhaustArt; }
		
		override public function primary() : void {
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
		
		override public function secondary() : void {x
			if (canFireSecondary) {
				// Secondary weapon can fire! Let's do it. Fire() !!
				fireTimerSecondary.start();
				canFireSecondary = false;
				fire();
			}
			else {
				// Secondary weapon on cooldown. No firing allowed.
			}
		}
		
		public function fire() : void {
			var m:MuzzleFlash = new MuzzleFlash(this);
			var l:Laser = new Laser(stageRef, this, -1, firePowerPrimary);			
			//this.x - _boundingbox.width / 2, this.y - this._boundingbox.height / 2
		
			l.x = this.x + getWeaponDistance().x;
			l.y = this.y + getWeaponDistance().y;
			l.alignPivot( "left", "center");
			l.scaleX = -1;
			
			m.alignPivot();
			m.x = getWeaponDistance().x;
			m.y = getWeaponDistance().y;
			m.scaleX = -1;
			
			this.addChild(m);
			laserSound.play(0,0, VARS.soundVolume);
			stageRef.addChild(l);
		}
		
		// Interface specific getters / setters
		// **************************************
		// GETTERS 
		
		public function get canFirePrimary() : Boolean { return _canFirePrimary; }
		public function get fireTimerPrimary() : Timer { return _fireTimerPrimary; }
		public function get fireSpeedPrimary() : Number { return _fireSpeedPrimary; }
		public function get firePowerPrimary() : int { return _firePowerPrimary; }
		
		public function get canFireSecondary() : Boolean { return _canFireSecondary; }
		public function get fireTimerSecondary() : Timer { return _fireTimerSecondary; }
		public function get fireSpeedSecondary() : Number { return _fireSpeedSecondary; }
		public function get firePowerSecondary() : int { return _firePowerSecondary; }
		
		// SETTERS
		
		public function set canFirePrimary(b:Boolean) : void { _canFirePrimary = b; }
		public function set fireTimerPrimary(t:Timer) : void { _fireTimerPrimary = t; }
		public function set fireSpeedPrimary(n:Number) : void { _fireSpeedPrimary = n; }
		public function set firePowerPrimary(i:int) : void { _firePowerPrimary = i; }
		
		public function set canFireSecondary(b:Boolean) : void { _canFireSecondary = b; }
		public function set fireTimerSecondary(t:Timer) : void { _fireTimerSecondary = t; }
		public function set fireSpeedSecondary(n:Number) : void { _fireSpeedSecondary = n; }
		public function set firePowerSecondary(i:int) : void { _firePowerSecondary = i; } 	
	}
}