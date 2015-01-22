package objects
{
	import debugger.Debug;
	
	import starling.display.Image;
	import starling.display.Sprite;
	
	public class Portrait extends Sprite
	{
		private var background:Image;
		private var _face:Image;
		
		public function Portrait()
		{
			super();
			background = new Image(Assets.getAtlas().getTexture("portraitbackground"));
			this.addChild(background);
		}
		
		public function set face(s:String) : void {
			Debug.INFO("Setting face to: '" + s + "'", this); 
			_face = new Image(Assets.getAtlas().getTexture(s));
			if (!this.contains(_face)) {
				this.addChild(_face);
				_face.x= 4;
				_face.y = 4;
			}
		}
	}
}