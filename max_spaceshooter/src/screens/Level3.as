package screens {
	import flash.events.Event;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.media.Sound;
	
	import controllers.KeyboardHandler;
	
	import debugger.Debug;
	
	import events.NavigationEvent;
	
	import global.VARS;
	
	import objects.Bumblebee;
	import objects.Drone;
	import objects.IngameBackground;
	import objects.Laser;
	import objects.Phaser;
	import objects.Screecher;
	import objects.ShieldHit;
	import objects.Ship;
	import objects.Sparrow;
	import objects.SuperPhaser;
	import objects.SuperScreecher;
	import objects.Target;
	
	import starling.animation.Transitions;
	import starling.animation.Tween;
	import starling.core.Starling;
	import starling.display.Image;
	import starling.display.MovieClip;
	import starling.events.Event;
	import starling.utils.deg2rad;
	
	import utils.Formatting;
	import utils.TextGenerator;

	public class Level3 extends Level {
		
		// Objects to be handled 
		private var player:Ship;
		private var commander:Bumblebee;
		private var GO:Image;
		private var bg:IngameBackground;
		private var boss:Ship;
		
		// Objective parameters
		private var currObjective:String = "";
		private var targetCount:uint;
		private var killCount:uint;
		private var comingEnemies:Array = new Array();
		private var comingPositions:Array = new Array();
		private var iterator:int = 0;
		private var nextSpawnFrame:Array = new Array();
		
		// Text box and handling
		private var textHandler:TextGenerator = new TextGenerator(this);
		
		// Key handling
		private var keyHandler:controllers.KeyboardHandler = new KeyboardHandler(this);

		// Dimensions of screen
		private var xMax:int;
		private var yMax:int;
		
		// this
		private var stageRef:Level = this;
		
		// Animation stuff
		private var shakeIterator:int = 0;

		
		// Constructor for second level
		public function Level3() {
			super();
			
			addEventListener(starling.events.Event.ADDED_TO_STAGE, onAddedToStage);
		}
		
		// This happens when screen is added to stage
		private function onAddedToStage(e:starling.events.Event) : void {
			removeEventListener(starling.events.Event.ADDED_TO_STAGE, onAddedToStage);
			xMax = this.stage.stageWidth - 100;
			yMax = this.stage.stageHeight - 100;
			init();
		}
		
		// Shake screen. Explosion? Earth quake? One does not know
		public function shakeScreen() : void {
			Debug.INFO("Shaking screen!", this);
			addEventListener(starling.events.Event.ENTER_FRAME, shakeLoop);
		}
		
		// Handle the shaking. Will it end? (yes, after 2 second)
		private function shakeLoop(e:starling.events.Event) : void {
			if (shakeIterator >= 90){
				shakeIterator = 0;
				removeEventListener(starling.events.Event.ENTER_FRAME, shakeLoop);
				this.x = 0.0;
				this.y = 0.0;
				this.scaleX = 1.0;
				this.scaleY = 1.0;
				this.dispatchEvent(new starling.events.Event("shakeDone"));
				return;
			}
			
			this.x = Math.random()*6 - 3;
			this.y = Math.random()*6 - 3;
			this.scaleX = 1 + Math.random() * 0.01;
			this.scaleY = 1 + Math.random() * 0.01;
			shakeIterator++;
		}
		
		// Init this screen!
		private function init() : void {
			drawScreen();	
			
			gameState = "player_enters";
			
			VARS.currProgress = -1;
			VARS.storyProgress = 0;
			
			ships.push(player);
			stage.addEventListener(starling.events.Event.ENTER_FRAME, gameLoop);
		}
		
		// Compose the graphical objects
		private function drawScreen() : void {
			bg = new IngameBackground(this, 3);
			player = new Sparrow(this);
			commander = new Bumblebee(this);
			chatter = commander;
			
			player.x = stage.stageWidth + player.width + 120;
			player.y = stage.stageHeight * 0.2;
			
			commander.x = 760;
			commander.y = stage.stageHeight * 0.6;
			
			textHandler.y = stage.stageHeight;
			
			// Add them all
			addChild(bg);
			addChild(textHandler);
			addChild(player);
			addChild(commander);
		}
		
		private function gameLoop(e:starling.events.Event) : void {
			switch (_gameState) {
				case "idle":
					break;
				case "player_enters":
					if (player.x > stage.stageWidth - player.width) {
						player.velX -= player.speed;
						
						if(Math.abs(player.velY) > player.maxVel/3) player.velY = (Math.abs(player.velY) / player.velY) * player.maxVel/3; 	// |velY| / velY -> 1 or -1
						if(Math.abs(player.velX) > player.maxVel/3) player.velX = (Math.abs(player.velX) / player.velX) * player.maxVel/3;	// |velX| / velX -> 1 or -1
						
						player.velX *= global.VARS.airRes;
						player.velY *= global.VARS.airRes;
						
						player.x += player.velX;
						
						if (!player.rotatable) break;
						
						if(player.velX < 4) {
							player.rotation = starling.utils.deg2rad(2 * player.velX);
							//player.exhaustArt.rotation = starling.utils.deg2rad(2 * -player.velX);
						}
						else {
							player.rotation = starling.utils.deg2rad(2 * 4 * (Math.abs(player.velX) / player.velX));
							//player.exhaustArt.rotation = starling.utils.deg2rad(2 * 4 * -(Math.abs(player.velX) / player.velX));
						}
					}
					else {
						gameState = "commander_enters";
					}
					break;
				case "commander_enters":
					var allInPlace:Boolean = true;
					addChild(commander);
					
					if (commander.x > stage.stageWidth - 200) {
						allInPlace = false;
						commander.velX -= commander.speed;
						
						if(Math.abs(commander.velX) > commander.maxVel/3) commander.velX = (Math.abs(commander.velX) / commander.velX) * commander.maxVel/3;
						
						commander.velX *= global.VARS.airRes;
						commander.x += commander.velX * 2;	
					}
					
					if (Math.round(player.y) > stage.stageHeight * 0.2 || Math.round(player.x) < stage.stageWidth - player.width) {
						allInPlace = false;
						
						if (Math.round(player.x) < stage.stageWidth - player.width) {
							
							player.velX += player.speed;
							
							if(Math.abs(player.velX) > player.maxVel/3) 
								player.velX = (Math.abs(player.velX) / player.velX) * player.maxVel/3;	// |velX| / velX -> 1 or -1
							
							player.velX *= global.VARS.airRes;
							player.x += player.velX;
							
						}
						if (Math.round(player.y) > stage.stageHeight * 0.2) {
							player.velY -= player.speed;
							
							if(Math.abs(player.velY) > player.maxVel/3)
								player.velY = (Math.abs(player.velY) / player.velY) * player.maxVel/3; 	// |velY| / velY -> 1 or -1
							
							player.velY *= global.VARS.airRes;
							player.y += player.velY;
						}
						
						if (!player.rotatable) break;
						
						if(player.velX < 4) {
							player.rotation = starling.utils.deg2rad(2 * player.velX);
							//player.exhaustArt.rotation = starling.utils.deg2rad(2 * -player.velX);
						}
						else {
							player.rotation = starling.utils.deg2rad(2 * 4 * (Math.abs(player.velX) / player.velX));
							//player.exhaustArt.rotation = starling.utils.deg2rad(2 * 4 * -(Math.abs(player.velX) / player.velX));
						}
					}
					
					if (allInPlace) {
						next();
					}
					
					break;
				case "phase_1":
					
					checkObjective();
					
					iterator++;
					
					
					if (iterator == 770) {
						ships.push(boss);
					}
					if (iterator == 950) {
						(boss as SuperScreecher).engage();
						iterator = 0;
						boss.alpha = 1;
					}					
					if (iterator >= 770) {
						boss.alpha += 1/180;
					}
					
					if (iterator >= 300 && iterator < 480) {
						boss.alpha -= 1/180;
					}
					
					if (iterator == 480) {
						ships.splice(ships.indexOf(boss), 1);
						(boss as SuperScreecher).idle();
										
						comingPositions.push(new Point(stage.stageWidth / 2 + 260, -84.75 / 2 + 10));
						comingPositions.push(new Point(stage.stageWidth / 2 + 130, -84.75 / 2 + 10));
						comingPositions.push(new Point(stage.stageWidth / 2, -84.75 / 2 + 10));
						comingPositions.push(new Point(stage.stageWidth / 2 - 130, -84.75 / 2 + 10));
						
						comingPositions.push(new Point(stage.stageWidth / 2 - 260, stage.stageHeight + 84.75 / 2));
						comingPositions.push(new Point(stage.stageWidth / 2 - 130, stage.stageHeight + 84.75 / 2));
						comingPositions.push(new Point(stage.stageWidth / 2, stage.stageHeight + 84.75 / 2));
						comingPositions.push(new Point(stage.stageWidth / 2 + 130, stage.stageHeight + 84.75 / 2));
					}
					
					switch(iterator) {
						case 510:
						case 530:
						case 550:
						case 570:
						case 680:
						case 700:
						case 720:
						case 740:
							spawnEnemy("Drone_fixed");
							break;
					}
					
					if (checkObjective()) {
						Debug.INFO("Objective completed!", this);
						this.dispatchEvent(new starling.events.Event("objectiveComplete"));
						break;
					}
					
					if(keyHandler.up) player.velY -= player.speed;
					if(keyHandler.down) player.velY += player.speed;
					if(keyHandler.left) player.velX -= player.speed;
					if(keyHandler.right) player.velX += player.speed;
					
					if(Math.abs(player.velY) > player.maxVel) player.velY = (Math.abs(player.velY) / player.velY) * player.maxVel; 	// |velY| / velY -> 1 or -1
					if(Math.abs(player.velX) > player.maxVel) player.velX = (Math.abs(player.velX) / player.velX) * player.maxVel;	// |velX| / velX -> 1 or -1
					
					player.velX *= global.VARS.airRes;
					player.velY *= global.VARS.airRes;
					
					if(Math.round(player.velX*10) == 0) player.velX = 0;
					if(Math.round(player.velY*10) == 0) player.velY = 0;
					
					player.x += player.velX;
					player.y += player.velY;
					
					if(player.x > this.xMax || player.x < player.width / 4) 
						player.x = (player.x > (stage.stageWidth / 2)) ? this.xMax : player.width / 4;
					
					if(player.y > this.yMax || player.y < player.height / 2)
						player.y = (player.y > (stage.stageHeight / 2)) ? this.yMax : player.height / 2;
					
					if (player.rotatable) {
						if(player.velX < 4) {
							player.rotation = starling.utils.deg2rad(2 * player.velX);
							(player as Sparrow).exhaustArt.rotation = starling.utils.deg2rad(2 * -player.velX);
						}
						else {
							player.rotation = starling.utils.deg2rad(2 * 4 * (Math.abs(player.velX) / player.velX));
							(player as Sparrow).exhaustArt.rotation = starling.utils.deg2rad(2 * 4 * -(Math.abs(player.velX) / player.velX));
						}
					}
					
					(player as Sparrow).exhaustArt.scaleX = -(player.velX / player.maxVel) + 1;
					
					// Handle weaponry
					if(keyHandler.fire1) player.primary();
					if(keyHandler.fire2) player.secondary();
					
					checkBullets();
					checkShipCollisions();
					
					break;
				
				case "fade_in":
					if (boss.alpha != 1) {
						boss.alpha += 1/180;
					} else {
						gameState = "over";
					}
					break;
				
				case "play":
					if (checkObjective()) {
						Debug.INFO("Objective completed!", this);
						this.dispatchEvent(new starling.events.Event("objectiveComplete"));
						break;
					}
					
					if(keyHandler.up) player.velY -= player.speed;
					if(keyHandler.down) player.velY += player.speed;
					if(keyHandler.left) player.velX -= player.speed;
					if(keyHandler.right) player.velX += player.speed;
					
					if(Math.abs(player.velY) > player.maxVel) player.velY = (Math.abs(player.velY) / player.velY) * player.maxVel; 	// |velY| / velY -> 1 or -1
					if(Math.abs(player.velX) > player.maxVel) player.velX = (Math.abs(player.velX) / player.velX) * player.maxVel;	// |velX| / velX -> 1 or -1
					
					player.velX *= global.VARS.airRes;
					player.velY *= global.VARS.airRes;
					
					if(Math.round(player.velX*10) == 0) player.velX = 0;
					if(Math.round(player.velY*10) == 0) player.velY = 0;
					
					player.x += player.velX;
					player.y += player.velY;
					
					if(player.x > this.xMax || player.x < player.width / 4) 
						player.x = (player.x > (stage.stageWidth / 2)) ? this.xMax : player.width / 4;
					
					if(player.y > this.yMax || player.y < player.height / 2)
						player.y = (player.y > (stage.stageHeight / 2)) ? this.yMax : player.height / 2;
					
					if (player.rotatable) {
						if(player.velX < 4) {
							player.rotation = starling.utils.deg2rad(2 * player.velX);
							(player as Sparrow).exhaustArt.rotation = starling.utils.deg2rad(2 * -player.velX);
						}
						else {
							player.rotation = starling.utils.deg2rad(2 * 4 * (Math.abs(player.velX) / player.velX));
							(player as Sparrow).exhaustArt.rotation = starling.utils.deg2rad(2 * 4 * -(Math.abs(player.velX) / player.velX));
						}
					}
					
					(player as Sparrow).exhaustArt.scaleX = -(player.velX / player.maxVel) + 1;
					
					// Handle weaponry
					if(keyHandler.fire1) player.primary();
					if(keyHandler.fire2) player.secondary();
					
					checkBullets();
					checkShipCollisions();
					
					break;
				
				case "final":
					
					if (++iterator % 120 == 0) 
						spawnEnemy("Drone");
					
					if (checkObjective()) {
						Debug.INFO("Objective completed!", this);
						this.dispatchEvent(new starling.events.Event("objectiveComplete"));
						break;
					}
					
					if(keyHandler.up) player.velY -= player.speed;
					if(keyHandler.down) player.velY += player.speed;
					if(keyHandler.left) player.velX -= player.speed;
					if(keyHandler.right) player.velX += player.speed;
					
					if(Math.abs(player.velY) > player.maxVel) player.velY = (Math.abs(player.velY) / player.velY) * player.maxVel; 	// |velY| / velY -> 1 or -1
					if(Math.abs(player.velX) > player.maxVel) player.velX = (Math.abs(player.velX) / player.velX) * player.maxVel;	// |velX| / velX -> 1 or -1
					
					player.velX *= global.VARS.airRes;
					player.velY *= global.VARS.airRes;
					
					if(Math.round(player.velX*10) == 0) player.velX = 0;
					if(Math.round(player.velY*10) == 0) player.velY = 0;
					
					player.x += player.velX;
					player.y += player.velY;
					
					if(player.x > this.xMax || player.x < player.width / 4) 
						player.x = (player.x > (stage.stageWidth / 2)) ? this.xMax : player.width / 4;
					
					if(player.y > this.yMax || player.y < player.height / 2)
						player.y = (player.y > (stage.stageHeight / 2)) ? this.yMax : player.height / 2;
					
					if (player.rotatable) {
						if(player.velX < 4) {
							player.rotation = starling.utils.deg2rad(2 * player.velX);
							(player as Sparrow).exhaustArt.rotation = starling.utils.deg2rad(2 * -player.velX);
						}
						else {
							player.rotation = starling.utils.deg2rad(2 * 4 * (Math.abs(player.velX) / player.velX));
							(player as Sparrow).exhaustArt.rotation = starling.utils.deg2rad(2 * 4 * -(Math.abs(player.velX) / player.velX));
						}
					}
					
					(player as Sparrow).exhaustArt.scaleX = -(player.velX / player.maxVel) + 1;
					
					// Handle weaponry
					if(keyHandler.fire1) player.primary();
					if(keyHandler.fire2) player.secondary();
					
					checkBullets();
					checkShipCollisions();
					
					break;
				
				case "phase_3":
					
					if (++iterator % 210 == 0) 
						spawnEnemy("Drone");
					
					if (checkObjective()) {
						Debug.INFO("Objective completed!", this);
						this.dispatchEvent(new starling.events.Event("objectiveComplete"));
						break;
					}
					
					if(keyHandler.up) player.velY -= player.speed;
					if(keyHandler.down) player.velY += player.speed;
					if(keyHandler.left) player.velX -= player.speed;
					if(keyHandler.right) player.velX += player.speed;
					
					if(Math.abs(player.velY) > player.maxVel) player.velY = (Math.abs(player.velY) / player.velY) * player.maxVel; 	// |velY| / velY -> 1 or -1
					if(Math.abs(player.velX) > player.maxVel) player.velX = (Math.abs(player.velX) / player.velX) * player.maxVel;	// |velX| / velX -> 1 or -1
					
					player.velX *= global.VARS.airRes;
					player.velY *= global.VARS.airRes;
					
					if(Math.round(player.velX*10) == 0) player.velX = 0;
					if(Math.round(player.velY*10) == 0) player.velY = 0;
					
					player.x += player.velX;
					player.y += player.velY;
					
					if(player.x > this.xMax || player.x < player.width / 4) 
						player.x = (player.x > (stage.stageWidth / 2)) ? this.xMax : player.width / 4;
					
					if(player.y > this.yMax || player.y < player.height / 2)
						player.y = (player.y > (stage.stageHeight / 2)) ? this.yMax : player.height / 2;
					
					if (player.rotatable) {
						if(player.velX < 4) {
							player.rotation = starling.utils.deg2rad(2 * player.velX);
							(player as Sparrow).exhaustArt.rotation = starling.utils.deg2rad(2 * -player.velX);
						}
						else {
							player.rotation = starling.utils.deg2rad(2 * 4 * (Math.abs(player.velX) / player.velX));
							(player as Sparrow).exhaustArt.rotation = starling.utils.deg2rad(2 * 4 * -(Math.abs(player.velX) / player.velX));
						}
					}
					
					(player as Sparrow).exhaustArt.scaleX = -(player.velX / player.maxVel) + 1;
					
					// Handle weaponry
					if(keyHandler.fire1) player.primary();
					if(keyHandler.fire2) player.secondary();
					
					checkBullets();
					checkShipCollisions();
					
					break;
				case "over":
					break;
				default:
					throw new Error("Unknown gameState: '" + _gameState + "'");
			}
		}
		
		// Initiate a new conversation
		private function startConvo() : void {
			var text:Array;
			switch (VARS.storyProgress++) {
				case 0:
					text = global.Conversations.get("commander_intro_2");
					textHandler.textBlocks = text;
					textHandler.startText();
					this.addEventListener("conversationOver", next);
					break;
				case 1:
					text = global.Conversations.get("boss_2_phase_1");
					textHandler.textBlocks = text;
					textHandler.startText();
					this.addEventListener("conversationOver", next);
					break;
				case 2:
					text = global.Conversations.get("boss_2_phase_2");
					textHandler.textBlocks = text;
					textHandler.startText();
					this.addEventListener("conversationOver", next);
					break;
				case 3:
					text = global.Conversations.get("boss_2_phase_3");
					textHandler.textBlocks = text;
					textHandler.startText();
					this.addEventListener("conversationOver", next);
					break;
				default:
					throw new Error("Unknown storyProgress: '" + VARS.storyProgress + "'");
			}
		}
		
		// This function is called when a conversation is over
		// to take the story forward
		override protected function storyTeller() : void {
			removeEventListener("conversationOver", next);
			Debug.INFO("storyTeller() - currProgress is: " + VARS.currProgress, this);
			var tween:Tween;
			
			switch (VARS.currProgress) {
				case 0:
					startConvo();
					gameState = "over";
					
					addEventListener("emp", function(e:starling.events.Event) : void {
						Debug.INFO("emp fired!", this);
						removeEventListener("emp", arguments.callee);
						
					});
					
					addEventListener("showEnemies", function(e:starling.events.Event) : void {
						boss = new SuperScreecher(stageRef);
						boss.x = 100;
						boss.y = 190;
						(boss as SuperScreecher).fixatePosition();
						(boss as SuperScreecher).init();
						Debug.INFO("showEnemies fired!", this);
						
						addChild(boss);
						boss.alpha = 0;
						
						var showLoop:Function;
						var alphaIterator:Number = 0;
						
						(new Assets.GameOverMusic()).play();
						
						var tween:Tween = new Tween(boss, 5, Transitions.LINEAR);
						tween.fadeTo(1);
						tween.onComplete = function() : void {
							removeEventListener(starling.events.Event.ENTER_FRAME, showLoop);	
							dispatchEvent(new starling.events.Event("shakeDone"));
							
							soundHandler = bossMusic.play();
							soundHandler.addEventListener(flash.events.Event.SOUND_COMPLETE, function(e:flash.events.Event) : void {
								soundHandler = bossMusic.play();
							});
						}
						Starling.juggler.add(tween);
					
					});
					
					var kc:Function;
					
					addEventListener("killCommander", kc = function(e:starling.events.Event) : void {
						removeEventListener("killCommander", kc);
						
						(boss as SuperScreecher).homeAndKill(commander);
						
						var mikeFire:Function;
						
						addEventListener("megaBlast", mikeFire = function() : void {
							
							removeEventListener("megaBlast", mikeFire);
							ships.push(commander);
							commander._HP = 1;
							
							(new Assets.LaserSound() as Sound).play(0, 0, global.VARS.soundVolume);
							
							var bulletCheck:Function;
							addEventListener(starling.events.Event.ENTER_FRAME, bulletCheck = function(e:starling.events.Event) : void {
								checkBullets();
							});
							
							addEventListener("enemyDown", function(e:starling.events.Event) : void {
								removeEventListener("enemyDown", arguments.callee);
								removeEventListener(starling.events.Event.ENTER_FRAME, bulletCheck);
								shakeScreen();
								addEventListener("conversationOver", next);
							});
						});
					});
					
					break;
				case 1:
					ships.push(boss);
					currObjective = "boss_first";
					addEventListener("objectiveComplete", function(e:starling.events.Event) : void {
						removeEventListener("objectiveComplete", arguments.callee);
						shakeScreen();
						currObjective = "ua";
						next();
						Starling.juggler.removeTweens(boss);
						
					});
					(boss as SuperScreecher).target = player;
					(boss as SuperScreecher).engage();
					gameState = "phase_1"; 
					
					break;
				case 2:
					
					(boss as SuperScreecher).idle();
					startConvo();
					gameState = "fade_in";
					
					break;
				case 3:
					currObjective = "killCount";
					
					addEventListener("objectiveComplete", function(e:starling.events.Event) : void {
						removeEventListener("objectiveComplete", arguments.callee);
						next();
					});
					
					addEventListener("enemyDown", function(e:starling.events.Event) : void {
						trace("I am called!");
						if (++killCount == targetCount) {
							removeEventListener("enemyDown", arguments.callee);
						}
						Debug.INFO(killCount + "/" + targetCount + " enemies defeated.", this);
					});
					
					comingEnemies = new Array("Phaser", "SuperPhaser", "SuperPhaser", "SuperPhaser");
					
					killCount = 0;
					targetCount = 5;
					iterator = 0;
					
					tween = new Tween(boss, 3, Transitions.LINEAR);
					tween.fadeTo(0);
					tween.onComplete = function() : void {	
						ships.splice(ships.indexOf(boss), 1);
						gameState = "play";
						spawnEnemy("Phaser");
					}
					Starling.juggler.add(tween);
					
					break;
				case 4:
					ships.push(boss);
					gameState = "play";
					currObjective = "boss_second";
					addEventListener("objectiveComplete", function(e:starling.events.Event) : void {
						removeEventListener("objectiveComplete", arguments.callee);
						shakeScreen();
						currObjective = "ua";
						Starling.juggler.removeTweens(boss);
						next();
					});
					(boss as SuperScreecher).target = player;
					
					tween = new Tween(boss, 3, Transitions.LINEAR);
					tween.fadeTo(1);
					tween.onComplete = function() : void {
						gameState = "phase_1";
						iterator = 0;
						
						(boss as SuperScreecher).engage();
					}
					Starling.juggler.add(tween);
					
					break;
				case 5:
					
					tween = new Tween(boss, 0.5, Transitions.LINEAR);
					tween.fadeTo(1)
					Starling.juggler.add(tween);
					gameState = "over";
					
					(boss as SuperScreecher).idle();
					startConvo();
					break;
				case 6:
					currObjective = "boss_third";
					addEventListener("objectiveComplete", function(e:starling.events.Event) : void {
						removeEventListener("objectiveComplete", arguments.callee);
						shakeScreen();
						currObjective = "ua";
						next();
					});
					(boss as SuperScreecher).target = player;
					(boss as SuperScreecher).superEngage();
					gameState = "phase_3"; 
					
					break;
				
				case 7:
					(boss as SuperScreecher).idle();
					gameState = "over";
					startConvo();
					break;
				case 8:
					
					(boss as SuperScreecher).superEngage();
					gameState = "final";
					var gameWon:Function;
					
					addEventListener("bossDied", function(e:starling.events.Event) : void {
						gameState = "over";
					});
					
					addEventListener("bossKilled", gameWon = function (e:starling.events.Event) : void {
						var lvlComplete:Image = new Image(Assets.getAtlas().getTexture("lvl_complete"));
						lvlComplete.x = stage.stageWidth / 2 - lvlComplete.width / 2;
						lvlComplete.y = 200;
						addChild(lvlComplete);
						lvlComplete.alpha = 0;
						
						var t:Tween = new Tween(lvlComplete, 1);
						t.fadeTo(1);
						Starling.juggler.add(t);
						gameState = "over";
						shakeScreen();
						this.removeEventListener("bossKilled", gameWon);
						soundHandler.stop();
							
						var tween:Tween = new Tween(this, 5.0, Transitions.EASE_IN_OUT);
						tween.fadeTo(0);    // equivalent to 'animate("alpha", 0)'
						tween.onComplete = showCredits;
						starling.core.Starling.juggler.add(tween);
					});
					
					break;
			}
		}
		
		private function showCredits() : void {
			var bg:Image = new Image(Assets.getTexture("EndingCredits"));
			this.parent.addChild(bg);
			bg.alpha = 0;
			var ite:uint = 0;
			
			var creditLoop:Function;
			
			addEventListener(starling.events.Event.ENTER_FRAME, creditLoop = function(e:starling.events.Event) : void {
				if (ite++ < 180) {
					bg.alpha += 1/180;
				} else if (ite > 400 && ite < 580) {  
					bg.alpha -= 1/180;
				} else if (ite == 580) {
					dispatchEvent(new NavigationEvent(NavigationEvent.CHANGE_SCREEN, {id: "lvlSelection"}, true));
					this.parent.removeChild(bg);
					Level.soundHandler.stop();
					Level.soundHandler = (new Assets.MenuMusic()).play();
				}
			});
		}
		
		// Spawn enemy of type 'type'
		private function spawnEnemy(type:String, countsTowardsObjective:Boolean = true) : void {
			var en:Ship;
			
			switch(type) {
				case "Phaser": 
					en = new Phaser(this, player);
					en.x = -en.width;
					en.y = Math.random()* (stage.stageHeight - 100) + 50;
					en.init();
					ships.push(en);
					addChild(en);
					
					break;
				
				case "SuperPhaser":
					en = new SuperPhaser(this, player);
					en.x = -en.width;
					en.y = Math.random()* (stage.stageHeight - 100) + 50;
					en.init();
					ships.push(en);
					addChild(en);
					
					break;
				case "Target":
					var t:Target = new Target(this);
					t.x = 40;
					t.y = Math.random() * (stage.stageHeight - (textHandler.height + 60)) + 60;
					t.fixatePosition();
					t.init();
					ships.push(t);
					addChild(t);
					break;
				case "Drone":
					var upper:Boolean = (Math.round(Math.random()) == 1);//(player.y < stage.stageHeight / 2);
					
					en = new Drone(this, countsTowardsObjective);
					en.x = player.x;
					en.y = upper ? (stage.stageHeight + en.height / 2 - 10) : -en.height + 10 ;
					ships.push(en);
					addChild(en);
					(en as Drone).setDirection(upper);
					break;
				
				case "Drone_fixed":
					en = new Drone(this);
					var pos:Point = new Point(); 
					pos.x = (comingPositions[0] as Point).x;
					pos.y = (comingPositions[0] as Point).y;
					comingPositions.splice(0, 1);
					Debug.INFO( "comingPos = ( " + pos.x + ", " + pos.y + ")", this);
					en.x = pos.x;
					en.y = pos.y;
					ships.push(en);
					addChild(en);
					(en as Drone).setDirection(pos.y > stage.stageHeight / 2);
					break;
				default: 
					throw new Error("spawnEnemy() received unknown type request: '" + type + "'");
			}
		}
		
		
		private function checkObjective() : Boolean {
			switch (currObjective) {
				case "killCount":
					if (killCount == targetCount) {
						return true;
					}
					break;
				case "boss_first":
					if (boss._HP <= 5000) {
						return true;
					}
					break;
				case "boss_second":
					if (boss._HP <= 3000) {
						return true;
					}
					break;
				case "boss_third":
					return (boss._HP <= 1000);
				case "ua":
					return false;
				default:
					throw new Error("Unknown objective type: '" + currObjective + "'");
			}
			
			return false;
		}
		
		
		// Player died - game over!
		private function gameOver() : void {
			
			this.removeEventListener(starling.events.Event.ENTER_FRAME, gameLoop);
			
			showGameOverScreen();
			(boss as SuperScreecher).disable();
			gameState = "over";
			(new Assets.GameOverMusic()).play();
		}
		
		// Let the player know that the game actually is over.
		private function showGameOverScreen() : void {
			GO = new Image(Assets.getAtlas().getTexture("gameOver"));
			GO.x = 450 - GO.width / 2;
			GO.y = 120;
			
			addChild(GO);
			Level.soundHandler.stop();
			
			var tween:Tween = new Tween(this, 6.0, Transitions.EASE_IN_OUT);
			tween.fadeTo(0);    // equivalent to 'animate("alpha", 0)'
			tween.onComplete = tween.onComplete = function () : void {
				dispatchEvent(new NavigationEvent(NavigationEvent.CHANGE_SCREEN, {id: "lvlSelection"}, true));
				Level.soundHandler = (new Assets.MenuMusic()).play();
			}
			
			starling.core.Starling.juggler.add(tween);	
			
			//drawGame();
		}
		
		
		// Check for collisions!
		private function checkShipCollisions() : void { 
			for (var i:uint = 0; i < ships.length; ++i) {
				//Debug.INFO(utils.Formatting.getName(ships[i]), this);
				if (ships[i] == player || utils.Formatting.getName(ships[i]) == "SuperScreecher") 
					continue;
				
				
				if ((ships[i] as Ship).boundingbox.intersects(player.boundingbox)) { 
					player.velY += (ships[i] as Drone).speed * 15; 
					if (player.takeDmg(45)) {
						gameOver();
						(new Assets.ExplosionSound() as Sound).play(0, 0, global.VARS.soundVolume);
						return;
					}
					(ships[i] as Drone).explode();
					
					var shieldHit:ShieldHit = new ShieldHit(player);
					shieldHit.x += player.direction * player.boundingbox.width / 2;
					shieldHit.y -= player.boundingbox.height / 2 + 10;
					
					shieldHit.scaleX = -player.direction * ((player.boundingbox.width + 20) / shieldHit.width);
					shieldHit.scaleY = (player.boundingbox.height + 20) / shieldHit.height;
					
					player.addChild(shieldHit);
				}
			}
		}
		
		// Check for hits!
		private function checkBullets() : void {
			for (var i:int = 0; i < bullets.length; ++i) {
				for (var j:int = 0; j < ships.length; ++j) {
					if ((bullets[i] as Laser).gunman == ships[j]) {
						// The one we hit was the one self. Not likely - skip this cycle.
						// This is a bounding box issue and handled by this simple hack.
						continue;
					}
					
					if (new Rectangle(bullets[i].x, bullets[i].y, (bullets[i] as Laser).hitBox.width, (bullets[i] as Laser).hitBox.width).intersects((ships[j] as Ship).boundingbox)) {
						//Debug.INFO("Hit was registered: on object [" + Formatting.getName(ships[j]) + "]", this);
						
						var isPlayer:Boolean = (ships[j] == player);
						
						if (ships[j].takeDmg((bullets[i] as Laser).power)) {
							// if this is player ship, game is over.
							if (isPlayer) {
								gameOver();
							}
							else {
								//spawnEnemy(Math.round(Math.random()) == 1 ? "Phaser" : "Target");
								this.dispatchEvent(new starling.events.Event("enemyDown"));
								if (comingEnemies.length != 0) {
									spawnEnemy(comingEnemies.splice(0, 1));
									//retargetEnemies(true);
								}
								else {
									
								}
							}
							
							(bullets[i] as Laser).destroy();
							// if the ship is destroyed, don't show shieldHit.
							break;
						}
						
						if (isPlayer && (bullets[i] as Laser).bigBlast)
							shakeScreen();
						
						(bullets[i] as Laser).destroy();
						
						var shieldHit:ShieldHit = new ShieldHit(ships[j]);
						shieldHit.x += ships[j].direction * ships[j].boundingbox.width / 2;
						shieldHit.y -= ships[j].boundingbox.height / 2 + 10;
						
						shieldHit.scaleX = -ships[j].direction * ((ships[j].boundingbox.width + 20) / shieldHit.width);
						shieldHit.scaleY = (ships[j].boundingbox.height + 20) / shieldHit.height;
						
						ships[j].addChild(shieldHit);
						
						(new Assets.ShieldHitSound()).play(0, 0, VARS.soundVolume);
						
						break;
					}
				}
			}
		}
	}
}