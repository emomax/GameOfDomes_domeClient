package objects
{
	import com.freeactionscript.effects.explosion.AbstractExplosion;
	import com.freeactionscript.effects.explosion.LargeExplosion;
	
	import flash.events.TimerEvent;
	import flash.geom.Point;
	import flash.media.Sound;
	import flash.utils.Timer;
	
	import Assets;
	
	import debugger.Debug;
	
	import global.VARS;
	
	import objects.ENEMY;
	import objects.Laser;
	import objects.MuzzleFlash;
	import objects.PrimaryWeapon;
	import objects.Ship;
	
	import screens.Level;
	
	import starling.display.Image;
	import starling.display.MovieClip;
	import starling.display.Sprite;
	import starling.events.Event;
	import starling.utils.deg2rad;
	
	public class Screecher extends Ship implements ENEMY, PrimaryWeapon
	{
		public var _playerRef:Ship;
		private var startY:Number;
		private var iterator:Number;
		private var hoverIterator:int = 0;
		
		public var homingBeamTop:Image = new Image(Assets.getAtlas().getTexture("homingLaser"));
		public var homingBeamBot:Image = new Image(Assets.getAtlas().getTexture("homingLaser"));
		
		private var offsetInRadians:Number = 0;
		
		private var box5:MovieClip;
		
		private var _bossState:String = "";
		private var canDoAction:Boolean = false;
		private var moves:Array = new Array();
		
		private var actionTimer:Timer;
		
		private var _fireSpeed:Number;
		private var _fireTimer:Timer;
		private var _firePower:int
		private var _canFire:Boolean;
		
		private var _coolDown:Boolean;
		private var _coolDownTimer:Timer;
		
		private var explodeIterator:int = 0;
		private var explosionSound:Sound = new Assets.ExplosionSound();
		private var shotIterator:int = 0;
		private var lastFire:Boolean = false;
		private var superMode:Boolean = false;
		
		public function Screecher(s:Sprite)
		{
			Debug.INFO("I AM ALIVE!", this);
			super(s);
			this._HP = 2300;
			this.moves.push("homeAndKill");
			this.moves.push("shootTriple");
			
			bossState = "idle";
			
			firePowerPrimary = 22;
			
			actionTimer = new Timer(3000, 1);
			actionTimer.addEventListener(TimerEvent.TIMER, onActionTimer);
			
			fireTimerPrimary = new Timer(100, 1);
			fireTimerPrimary.addEventListener(TimerEvent.TIMER, onFireTimer);
			
			_coolDownTimer = new Timer(1000, 1);
			_coolDownTimer.addEventListener(TimerEvent.TIMER, onCoolDownTimer);
			_coolDown = true;
			canFirePrimary = true;
		}
		
		private function onActionTimer(e:TimerEvent) : void {	
			canDoAction = true;
			actionTimer.stop();		
		}
		
		private function onFireTimer(e:TimerEvent) : void {
			canFirePrimary= true;
			fireTimerPrimary.stop();
		}
		
		private function onCoolDownTimer(e:TimerEvent) : void {
			_coolDown = true;
			_coolDownTimer.stop();
		}
		
		override protected function onAddedToStage(e:Event) : void {
			// Middle
			this.createExhaustArt(new Point(-70, -15), -1);
			this.createExhaustArt(new Point(-71, -10), -1);
			this.createExhaustArt(new Point(-71, -5), -1);
			
			this.createExhaustArt(new Point(-72, 0), -1);
			this.createExhaustArt(new Point(-70, 5), -1);
			this.createExhaustArt(new Point(-72, 10), -1);
			
			// Upper
			this.createExhaustArt(new Point(-71, -45), -1);
			this.createExhaustArt(new Point(-72, -40), -1);
			this.createExhaustArt(new Point(-70, -35), -1);

			this.createExhaustArt(new Point(-69, -20), -1);
			this.createExhaustArt(new Point(-70, -25), -1);
			this.createExhaustArt(new Point(-71, -30), -1);
			
			// Bottom
			this.createExhaustArt(new Point(-70, 15), -1);
			this.createExhaustArt(new Point(-72, 20), -1);
			this.createExhaustArt(new Point(-69, 25), -1);

			this.createShipArt("screecher");
			
			this.setWidth(this.shipArt.width);
			Debug.INFO("getWidth() returns: " + this.getWidth(), this);
			Debug.INFO("this.width returns: " + this.width, this);
			
			this.boundingbox = this.bounds;
			
			this.addChild(homingBeamTop);
			homingBeamTop.y = -122;
			homingBeamTop.x = 66;
			homingBeamTop.pivotY = homingBeamTop.height / 2;
			
			this.addChild(homingBeamBot);
			homingBeamBot.y = 83;
			homingBeamBot.x = 70;
			homingBeamBot.pivotY = homingBeamTop.height / 2;
			
			homingBeamBot.visible = false;
			homingBeamTop.visible = false;
			
			//addChild(box5);
			
			this.showBoundingBox();
			direction = 1;
		}
		
		public function fixatePosition() : void {
			this.startY = this.y;
		}
		
		public function getBeams(theta1:Number, theta2:Number) : Array {
			
			var a:Array = new Array();
			var a1:Array = new Array();
			var a2:Array = new Array();
			
			a1.push(new Point(homingBeamTop.x, homingBeamTop.y));
			a1.push(new Point(homingBeamTop.x + homingBeamTop.width, homingBeamTop.y + homingBeamTop.height * ((Math.sin(theta1) <= 0) ? -1 : 1)));
				
			a2.push(new Point(homingBeamBot.x, homingBeamBot.y));
			a2.push(new Point(homingBeamBot.x + homingBeamBot.width, homingBeamBot.y + homingBeamBot.height * ((Math.sin(theta2) <= 0) ? 1 : -1)));
			
			a.push(a1);
			a.push(a2);
			
			return a;
		}
		
		override public function init() : void {
			// Init the Phaser!
			Debug.INFO("I am initiated!", this);
			addEventListener(Event.ENTER_FRAME, behaviourLoop);
			
			//box5 = new DrawRect(new Rectangle(0, 0, 10, 10));
			/*box5 = new MovieClip(Assets.getAtlas().getTextures("spark_"), 15);
			box5.alignPivot();
			box5.scaleX = 0.6;
			box5.scaleY = 0.76;
			box5.visible = false;
			starling.core.Starling.juggler.add(box5);*/
		}
		
		public function homeAndKill(t:Ship) : void {
			Debug.INFO("homeAndKill()", this);
			iterator = 0;
			
			homingBeamTop.visible = true;
			homingBeamBot.visible = true;
			
			this._playerRef = t;
			
			//box5.x = t.x - this.x;
			//box5.y = t.y - this.y;
			
			iterator = 0;
			
			var u:Point = new Point (-this.x - homingBeamTop.x + _playerRef.x, -this.y - homingBeamTop.y + _playerRef.y);
			var v:Point = new Point (-this.x - homingBeamBot.x + _playerRef.x, -this.y - homingBeamBot.y + _playerRef.y);
			
			var theta1:Number = Math.atan(u.y / u.x);
			var theta2:Number = Math.atan(v.y / v.x);
			
			homingBeamTop.rotation = theta1;
			homingBeamBot.rotation = theta2;
			
			bossState = "homing";
		}
		
		private function behaviourLoop(e:Event) : void {
			// Do stuff;
			
			switch (_bossState) {/**/
				case "laserTag":
					iterator++;
					
					if (iterator == 1) {
						homingBeamTop.visible = true;
						homingBeamBot.visible = true;
						homingBeamTop.rotation = 0;
						homingBeamBot.rotation = 0;
					} else if (iterator == 10){
						homingBeamTop.visible = false;
						homingBeamBot.visible = false;
					} else if (iterator == 20) {
						homingBeamTop.visible = true;
						homingBeamBot.visible = true;
					} else if (iterator == 30) {
						homingBeamTop.visible = false;
						homingBeamBot.visible = false;
					} else if (iterator >= 40) {
						bossState = "fireTriplet";
					}
					break;
				case "homing":					
					iterator += superMode ? 2 : 1;
					
					if (iterator <= 2) {
						//box5.visible = true;
						homingBeamTop.visible = true;
						homingBeamBot.visible = true;
					} else if (iterator == 20){
						homingBeamTop.visible = false;
						homingBeamBot.visible = false;
						//visible = false;
					} else if (iterator == 40) {
						
						//box5.visible = true;
						homingBeamTop.visible = true;
						homingBeamBot.visible = true;
					} else if (iterator == 60) { 
						homingBeamTop.visible = false;
						homingBeamBot.visible = false;
						//box5.visible = false;
					} else if (iterator == 90) {
						fire();
						bossState = "idle";
						stageRef.dispatchEvent(new Event("megaBlast"));
						startActionTimer();
						Debug.INFO("FIRE", this);
					}
					break;
				
				case "fireTriplet":
					if (canFirePrimary) {							
						if (shotIterator++ <3) {									
							lastFire = !lastFire;
							fire(true);
							
							fireTimerPrimary.start();
							canFirePrimary = false;
						} else {
							shotIterator = 0;
							canDoAction = false;
							startActionTimer();
							stageRef.dispatchEvent(new Event("megaBlast"));
						}
					}
					break;
					
				case "engagedInCombat": 
					if (canDoAction) {
						
						var attackState:String = getAttackState();
					
						switch (attackState) {
							case "homeAndKill":
								stageRef.addEventListener("megaBlast", engage);
								homeAndKill(target);
								canDoAction = false;
								
								break;
							case "shootTriple": 
								stageRef.addEventListener("megaBlast", engage);
								bossState = "laserTag";
								iterator = 0;
								break;
							default: 
								throw new Error("Unknown move for boss Screecher: " + attackState);
						}
					}
					else { /*  candonothing */}
				
				case "idle":
					var hoverRadians:Number = deg2rad(hoverIterator++);
					this.y = startY + 30 * Math.sin(hoverRadians);	
					
					homingBeamTop.visible = false;
					homingBeamBot.visible = false;
					//box5.visible = false;
					break;
					
				case "player_dead":
					removeEventListener(Event.ENTER_FRAME, arguments.callee);
					break;
								
				default:
					// fall through loop
					throw new Error("Unknown bossState: '" + bossState + "'");
					break;
			}
		}
		
		public function engage(e:Event = null) : void {
			if (e != null ) {
				stageRef.removeEventListener("megaBlast", arguments.callee);
			} 
			
			bossState = "engagedInCombat";
		}
		
		public function superEngage() : void {
			
			bossState = "engagedInCombat";
			actionTimer.stop();
			actionTimer = new Timer(600, 1);
			actionTimer.addEventListener(TimerEvent.TIMER, onActionTimer);
			actionTimer.reset();
			//actionTimer.start();
			superMode = true;
		}
		
		public function disable() : void {
			Debug.INFO( "disable() called! ", this);
			stageRef.removeEventListener("megaBlast", engage);
			bossState = "idle";

			_coolDownTimer.stop();
			fireTimerPrimary.stop();
			actionTimer.stop();
		}
		
		public function idle() : void {
			bossState = "idle";
			
			startActionTimer();
		}
		
		public function EMP() : void {
			bossState = "player_dead";
		}
		
		public function startActionTimer() : void {
			actionTimer.start();
		}
		
		private function getAttackState() : String {
			switch (VARS.currProgress) {
				case 9:
					return moves[0];
					break;
				case 11: 
					return moves[1];
					break;
				case 13:
					var i:int = (Math.ceil(Math.random()*moves.length) - 1);
					return moves[i];
				default:
					throw new Error("Unhandled VARS.currProgress from Screecher: " + VARS.currProgress);
			}
		}
		
		private function set bossState(state:String) : void {
			_bossState = state;
			Debug.INFO("bossState is now: " + state, this);
		}
		
		public function clean() : void {}
		
		
		private function get bossState() : String {return _bossState;}
		public function get target() : Ship {return _playerRef;}
		public function set target(s:Ship) : void { _playerRef = s;}
		
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
		
		public function fire(normalShot:Boolean = false) : void {
			if (normalShot) {
				
				var m:MuzzleFlash = new MuzzleFlash(stageRef);
				var l:Laser = new Laser(stageRef, this, 1, firePowerPrimary, null, true);
				
				if (lastFire) {
					l.x = this.x + homingBeamTop.x;
					l.y = this.y + homingBeamTop.y;
					
					m.x = this.x + homingBeamTop.x;
					m.y = this.y + homingBeamTop.y;
					
				}
				else {
					l.x = this.x + homingBeamBot.x;
					l.y = this.y + homingBeamBot.y;
					
					m.x = this.x + homingBeamBot.x;
					m.y = this.y + homingBeamBot.y;
				}
				
				stageRef.addChild(m);
				stageRef.addChild(l);
				return;
			}
			var dir1:Point = new Point();
			var dir2:Point = new Point();
			
			dir1.x = Math.cos(homingBeamTop.rotation);
			dir1.y = Math.sin(homingBeamTop.rotation);
			
			dir2.x = Math.cos(homingBeamBot.rotation);
			dir2.y = Math.sin(homingBeamBot.rotation);
			
			m = new MuzzleFlash(stageRef);
			l = new Laser(stageRef, this, 1, firePowerPrimary, dir1, true);
			var m2:MuzzleFlash = new MuzzleFlash(stageRef);
			var l2:Laser = new Laser(stageRef, this, 1, firePowerPrimary, dir2, true);
			
			l.scaleX = 2;
			l.scaleY = 2;
			l2.scaleX = 2;
			l2.scaleY = 2;
			
			m.scaleX = 3;
			m.scaleY = 3;
			m2.scaleX = 3;
			m2.scaleY = 3;
			
			l.alignPivot("left","center");
			m.alignPivot("left", "center");
			l2.alignPivot("left", "center");
			m2.alignPivot("left", "center");
			
			l.rotation = homingBeamTop.rotation;
			l2.rotation = homingBeamBot.rotation;
			
			l.x = this.x + homingBeamTop.x;
			l.y = this.y + homingBeamTop.y;
			
			m.x = this.x + homingBeamTop.x;
			m.y = this.y + homingBeamTop.y;
			
			l2.x = this.x + homingBeamBot.x;
			l2.y = this.y + homingBeamBot.y;
			
			m2.x = this.x + homingBeamBot.x;
			m2.y = this.y + homingBeamBot.y;
			
			stageRef.addChild(m);
			stageRef.addChild(l);
			stageRef.addChild(m2);
			stageRef.addChild(l2);
		}
		
		override protected function destroy() : void {
			(stageRef as Level).ships.splice((stageRef as Level).ships.indexOf(this), 1);
			
			explodeIterator = 0;
			addEventListener(Event.ENTER_FRAME, explodeLoop);
			(stageRef as Level).fanfareSound.play();
			stageRef.dispatchEvent(new Event("bossDied"));
			bossState = "idle";
		}
		
		private function explodeLoop(e:Event) : void {
			var expl:AbstractExplosion = new LargeExplosion(stageRef);
			if (++explodeIterator > 210) {
				
				//Debug.INFO("Explode at: (" + (this.x - shipArt.width / 2 + (Math.random()*shipArt.width*2 - shipArt.width)) + ", " + (this.y + (Math.random()*500 - 250)) + ")", this);
				expl.create(this.x - 30, (this.y + shipArt.height / 4));
				expl.create(this.x - 20, (this.y - shipArt.height / 4));
				expl.create(this.x + 30, (this.y));
				explosionSound.play();
				dispatchEvent(new Event("bossKilled", true));
				stageRef.removeChild(this, true);
				removeEventListener(Event.ENTER_FRAME, explodeLoop);
				removeEventListener(Event.ENTER_FRAME, behaviourLoop);
			} else if (explodeIterator % 10 == 0 && explodeIterator < 160) {	
				this.alpha = 0.5;
				expl.create(this.x + (Math.random()*shipArt.width / 2  - shipArt.width/4) - shipArt.width / 4, (this.y + (Math.random()*shipArt.height /2- shipArt.height/4)));
				explosionSound.play(0, 0, VARS.soundVolume);
			}
			else {
				this.alpha = 1;
			}
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
	}
}