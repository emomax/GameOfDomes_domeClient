package screens
{
	import flash.media.Sound;
	import flash.media.SoundChannel;
	
	import debugger.Debug;
	
	import global.VARS;
	
	import objects.Ship;
	
	import starling.display.Sprite;
	
	public class Level extends Sprite
	{
		public var bullets:Array = new Array();
		public var ships:Array = new Array();
		
		protected var bossMusic:Sound = new Assets.BossMusic();
		protected var completeSound:Sound = new Assets.CompleteMusic();
		public var fanfareSound:Sound = new Assets.FanfareMusic();
		
		public static var soundHandler:SoundChannel = new SoundChannel();
		protected var chatter:Ship;
		protected var _gameState:String = "idle";
		
		public function Level()
		{
			super();
		}
		
		// Let's the game loop know where in the story we are.
		protected function set gameState(state:String) : void {
			_gameState = state;
			Debug.INFO("gameState is now: " + state, this);
		}
		
		// Show bounding boxes of all ships
		public function showBoxes() : void {
			for (var i:int = 0; i < ships.length; ++i) {
				(ships[i] as Ship).toggleBox(!VARS.showBoundingBoxes);
			}
			
			VARS.showBoundingBoxes = !VARS.showBoundingBoxes;	
		}
		
		// Take me to the next stage of this epic adventure!
		public function next() : void {
			Debug.INFO("next() incremented current progress!", this);
			//Debug.INFO("ships[] is: " + Debug.formatArray(ships), this);
			++VARS.currProgress;
			storyTeller();
		}
		
		public function showChatter() : void {
			chatter.showConvoBubble();
		}
		
		public function hideChatter() : void {
			if (chatter)
				chatter.hideConvoBubble();
		}
		
		// This function is called when a conversation is over
		// to take the story forward
		protected function storyTeller() : void {}
	}
}