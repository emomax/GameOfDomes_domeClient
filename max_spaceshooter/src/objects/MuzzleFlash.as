package objects
{
	import flash.utils.Timer;
	import flash.events.TimerEvent;
	import starling.display.Image;
	
	import starling.display.Sprite;
	
	public class MuzzleFlash extends Sprite
	{
		private var art:Image;
		private var timer:Timer = new Timer(30);
		
		private var stageRef:Sprite;
		
		public function MuzzleFlash(s:Sprite)
		{	
			art = new Image(Assets.getAtlas().getTexture("muzzleFlash_000"+(Math.round(Math.random()) + 1)));
			
			super();
			
			stageRef = s;
			addChild(art);
			
			timer.addEventListener(TimerEvent.TIMER, destroy);
			timer.start();
		}
		
		private function destroy(e:TimerEvent) : void {
			timer.removeEventListener(TimerEvent.TIMER, destroy);
			stageRef.removeChild(this, true);
		}
	}
}