/**
 * Dynamic Animated Explosion
 * Small Explosion
 * ---------------------
 * VERSION: 1.0
 * DATE: 8/22/2011
 * AS3
 * UPDATES AND DOCUMENTATION AT: http://www.FreeActionScript.com
 **/
package com.freeactionscript.effects.explosion 
{
	import flash.display.DisplayObjectContainer;
	import flash.filters.BitmapFilterQuality;
	import flash.filters.BitmapFilterType;
	import flash.filters.BlurFilter;
	import flash.filters.GradientGlowFilter;
	
	public class SmallExplosion extends AbstractExplosion
	{
		/**
		 * Explosion Constructor
		 * @param	container	Main Explosion effect container. Takes MovieClip, Sprite, Document Class as argument.
		 */
		function SmallExplosion(container:DisplayObjectContainer) 
		{
			super(container);
		}
		
		/**
		 * This function sets explosion settings
		 */
		override protected function setStats():void
		{
			_numberOfFireballs = 6;
			_explosionRange = 15;
			_growScale = Math.random() * 100 * .005;
			_growSpeed = .1
			_growAlpha = Math.random() * 200 * .01;
			_fadeSpeed = .1
			_randomRangeX = Math.random() * _explosionRange - _explosionRange * .5;
			_randomRangeY = Math.random() * _explosionRange - _explosionRange * .5;
			_randomNumber = Math.round(Math.random() * 1);
			_speedX = 0;
			_speedY = 0;
			_randomBlur = Math.random() * 3 + 3;
		}
		
		/**
		 * Create light at X & Y
		 * @param	targetX
		 * @param	targetY
		 */
		override protected function createLight(targetX:Number, targetY:Number):void
		{
			_tempParticle = new Particle(new Light());
			
			// set particle properties
			_tempParticle.x = targetX;
			_tempParticle.y = targetY;
			_tempParticle.scaleX = 1.5;
			_tempParticle.scaleY = 1.5;
			_tempParticle.alpha = .5;
			_tempParticle.growSpeed = .5;
			_tempParticle.fadeSpeed = .1;
			_tempParticle.vx = 0;
			_tempParticle.vy = 0;
			
			// add to display list
			_container.addChild(_tempParticle);
			
			// add to particle array
			_particles.push(_tempParticle);
		}
		
		/**
		 * Create fireball base at X & Y
		 * @param	targetX
		 * @param	targetY
		 */
		override protected function createFireballBase(targetX:Number, targetY:Number):void
		{
			_tempParticle = new Particle(new Fireball());
			
			// set particle properties
			_tempParticle.x = targetX;
			_tempParticle.y = targetY;
			_tempParticle.scaleX = 1;
			_tempParticle.scaleY = 1;
			_tempParticle.alpha = 1;
			_tempParticle.growSpeed = .2;
			_tempParticle.fadeSpeed = .1;
			_tempParticle.vx = 0;
			_tempParticle.vy = 0;
			
			// add to display list
			_container.addChild(_tempParticle);
			
			// add to particle array
			_particles.push(_tempParticle);
		}
		
		/**
		 * Create fireball burn mark at X & Y
		 * @param	targetX
		 * @param	targetY
		 */
		override protected function createBurnMark(targetX:Number, targetY:Number):void
		{
			_tempParticle = new Particle(new BurnMark());
			
			// set particle properties
			_tempParticle.x = targetX;
			_tempParticle.y = targetY;
			_tempParticle.scaleX = 1;
			_tempParticle.scaleY = 1;
			_tempParticle.alpha = Math.random() * .5 + .25;
			_tempParticle.growSpeed = 0;
			_tempParticle.fadeSpeed = 0;
			_tempParticle.vx = 0;
			_tempParticle.vy = 0;
			_tempParticle.rotation = Math.random() * 360;
			
			// add to display list
			_container.addChild(_tempParticle);
			
			// burn marks dont get added to _particles array
			//_particles.push(_tempParticle);
		}
		
	}

}