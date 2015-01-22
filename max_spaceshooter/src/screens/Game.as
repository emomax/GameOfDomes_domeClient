package screens 
{
	import flash.media.Sound;
	
	import debugger.Debug;
	
	import events.NavigationEvent;
	
	import screens.InGame;
	import screens.Level2;
	import screens.Level3;
	
	import starling.display.Sprite;
	import starling.events.Event;
	
	public class Game extends Sprite {
	
		private var inGame:Sprite;
		private var menu:Sprite = new Menu();
		private var lvlSelection:Sprite = new LevelSelection();
		private var lvlCompleted:Sprite;
		
		public function Game () : void {
			super();
			addEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
		}
		
		private function onAddedToStage(e:Event) : void {

			this.addChild(menu);
			this.addEventListener(NavigationEvent.CHANGE_SCREEN, changeScreen);
			
			Level.soundHandler = (new Assets.MenuMusic()).play();
		}
		
		private function changeScreen(e:NavigationEvent) : void {
			Debug.INFO("Change to screen: '" + e.params.id + "'", this);
			
			switch (e.params.id) {
				case "level_1":
					
					(new Assets.PlaySound()).play();
					lvlSelection.visible = false;
					lvlSelection.dispose();
					lvlSelection = new Sprite();
					inGame = new InGame();
					inGame.visible = true;
					Level.soundHandler.stop();
					addChild(inGame);
					break;
				case "level_2":
					
					(new Assets.PlaySound()).play();
					lvlSelection.visible = false;
					lvlSelection.dispose();
					lvlSelection = new Sprite();
					inGame = new Level2();
					inGame.visible = true;
					Level.soundHandler.stop();
					addChild(inGame);
					break;
				case "level_3":
					
					(new Assets.PlaySound()).play();
					lvlSelection.visible = false;
					lvlSelection.dispose();
					lvlSelection = new Sprite();
					inGame = new Level3();
					inGame.visible = true;
					Level.soundHandler.stop();
					addChild(inGame);
					break;
				case "lvlSelection":
					(new Assets.ClickSound()).play();
					menu.visible = false;
					inGame = new Sprite();
					inGame.visible = false;
					lvlSelection = new LevelSelection();
					lvlSelection.visible = true;
					(lvlSelection as LevelSelection).init();
					addChild(lvlSelection);
					break;
				case "level_completed":
					inGame.visible = false;
					lvlCompleted.visible = true;
					break;
			}
		}
	}
}