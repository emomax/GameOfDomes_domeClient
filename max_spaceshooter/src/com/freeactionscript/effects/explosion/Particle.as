/**
 * Dynamic Animated Explosion
 * Particle Object
 * ---------------------
 * VERSION: 1.0
 * DATE: 8/22/2011
 * AS3
 * UPDATES AND DOCUMENTATION AT: http://www.FreeActionScript.com
 **/
package com.freeactionscript.effects.explosion 
{
	import starling.display.Image;
	import starling.display.Sprite;
	
	public class Particle extends Sprite
	{
		// vars
		private var _graphic:Image;
		private var _growSpeed:Number;
		private var _fadeSpeed:Number;
		private var _vx:Number;
		private var _vy:Number;
		
		/**
		 * Particle Constructor
		 * @param	graphic	Takes a DisplayObjectContainer (MovieClip, Sprite, etc)
		 */
		public function Particle(graphic:Image) 
		{
			_graphic = graphic;
			
			addChild(_graphic);
		}
		
		//////////////////////////////////////
		// Getters and Setters
		//////////////////////////////////////
		
		public function get growSpeed():Number 
		{
			return _growSpeed;
		}
		
		public function set growSpeed(value:Number):void 
		{
			_growSpeed = value;
		}
		
		public function get fadeSpeed():Number 
		{
			return _fadeSpeed;
		}
		
		public function set fadeSpeed(value:Number):void 
		{
			_fadeSpeed = value;
		}
		
		public function get vx():Number 
		{
			return _vx;
		}
		
		public function set vx(value:Number):void 
		{
			_vx = value;
		}
		
		public function get vy():Number 
		{
			return _vy;
		}
		
		public function set vy(value:Number):void 
		{
			_vy = value;
		}
		
	}

}